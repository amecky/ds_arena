#include "ElasticBorder.h"
#include "..\shaders\Border_PS_Main.inc"
#include "..\shaders\Border_VS_Main.inc"

const float Tension = 0.025f;
const float Dampening = 0.025f;
const float Spread = 0.15f;

void initializeRibbon(Ribbon* ribbon, int num, const ds::vec2& startPos, const ds::vec2& dir, bool vertical, float size) {
	ribbon->points = new LinePoint[num];
	ribbon->lDeltas = new float[num];
	ribbon->rDeltas = new float[num];
	ribbon->num = num;
	ribbon->vertical = vertical;
	for (int i = 0; i < num; ++i) {
		ribbon->points[i].height = 40.0f;
		ribbon->points[i].targetHeight = 40.0f;
		ribbon->points[i].speed = 0.0f;
		ds::vec2 sp = ds::vec2(i * size, i * size);
		sp.x *= dir.x;
		sp.y *= dir.y;
		ribbon->points[i].pos = sp + startPos;
		ribbon->points[i].color = ds::Color(192, 0, 0, 255);
	}
}

void updateRibbon(Ribbon* ribbon) {	
	for (int i = 1; i < ribbon->num - 1; ++i) {
		LinePoint& lp = ribbon->points[i];
		float x = lp.targetHeight - lp.height;
		lp.speed += Tension * x - lp.speed * Dampening;
		lp.height += lp.speed;
	}

	// do some passes where columns pull on their neighbours
	for (int j = 0; j < 2; j++) {
		for (int i = 0; i < ribbon->num; i++) {
			if (i > 0) {
				ribbon->lDeltas[i] = Spread * (ribbon->points[i].height - ribbon->points[i - 1].height);
				ribbon->points[i - 1].speed += ribbon->lDeltas[i];
			}
			if (i < ribbon->num - 1) {
				ribbon->rDeltas[i] = Spread * (ribbon->points[i].height - ribbon->points[i + 1].height);
				ribbon->points[i + 1].speed += ribbon->rDeltas[i];
			}
		}

		for (int i = 0; i < ribbon->num; i++) {
			if (i > 0) {
				ribbon->points[i - 1].height += ribbon->lDeltas[i];
			}
			if (i < ribbon->num - 1) {
				ribbon->points[i + 1].height += ribbon->rDeltas[i];
			}
		}
	}
	// fix corner points
	ribbon->points[0].height = ribbon->points[0].targetHeight;
	ribbon->points[ribbon->num - 1].height = ribbon->points[ribbon->num - 1].targetHeight;
}
// ---------------------------------------------------
//
// AbstractBorder
//
// ---------------------------------------------------
ElasticBorder::ElasticBorder(float length, float thickness, int numX, int numY, const ds::vec4& texture, RID textureID) //, const ds::AABBox& box) 
	: _length(length) , _thickness(thickness), _texture(texture) , _textureID(textureID) {

	// bottom
	initializeRibbon(&_ribbons[0], numX, ds::vec2(40, 0), ds::vec2(1, 0), true, _length);
	// top
	initializeRibbon(&_ribbons[1], numX, ds::vec2(40, 615), ds::vec2(1, 0), true, _length);
	// left
	initializeRibbon(&_ribbons[2], numY, ds::vec2(0, 35), ds::vec2(0, 1), false, _length);
	// right
	initializeRibbon(&_ribbons[3], numY, ds::vec2(940, 35), ds::vec2(0, 1), false, _length);

	int total = numX * numY * 2;
	_vertices = new GridVertex[total * 4];

	// create orthographic view
	ds::matrix orthoView = ds::matIdentity();
	ds::matrix orthoProjection = ds::matOrthoLH(ds::getScreenWidth(), ds::getScreenHeight(), 0.1f, 1.0f);
	_orthoPass = ds::createRenderPass(orthoView, orthoProjection, ds::DepthBufferState::DISABLED);

	_constantBuffer.viewprojectionMatrix = ds::matTranspose(orthoView * orthoProjection);
	_constantBuffer.worldMatrix = ds::matTranspose(ds::matIdentity());

	RID stateGroup = createStateGroup(total * 4, textureID);
	ds::DrawCommand drawCmd = { 100, ds::DrawType::DT_INDEXED, ds::PrimitiveTypes::TRIANGLE_LIST, 0 };

	_drawItem = ds::compile(drawCmd, stateGroup);

	_timer = 0.0f;
}

RID ElasticBorder::createStateGroup(int numVertices, RID textureID) {

	RID bs_id = ds::createBlendState(ds::BlendStates::SRC_ALPHA, ds::BlendStates::SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, true);

	RID vertexShader = ds::createVertexShader(Border_VS_Main, sizeof(Border_VS_Main), "Border_VS");
	RID pixelShader = ds::createPixelShader(Border_PS_Main, sizeof(Border_PS_Main), "Border_PS");

	ds::VertexDeclaration decl[] = {
		{ ds::BufferAttribute::POSITION,ds::BufferAttributeType::FLOAT,3 },
		{ ds::BufferAttribute::TEXCOORD,ds::BufferAttributeType::FLOAT,2 },
		{ ds::BufferAttribute::COLOR,ds::BufferAttributeType::FLOAT,4 }
	};

	int q = numVertices / 4 * 6;

	RID rid = ds::createVertexDeclaration(decl, 3, vertexShader);
	RID cbid = ds::createConstantBuffer(sizeof(CubeConstantBuffer), &_constantBuffer);
	RID indexBufferID = ds::createQuadIndexBuffer(numVertices / 4);
	_vertexBufferID = ds::createVertexBuffer(ds::BufferType::DYNAMIC, numVertices, sizeof(GridVertex));
	RID ssid = ds::createSamplerState(ds::TextureAddressModes::CLAMP, ds::TextureFilters::LINEAR);


	RID stateGroup = ds::StateGroupBuilder()
		.inputLayout(rid)
		.vertexShader(vertexShader)		
		.constantBuffer(cbid, vertexShader, 0)
		.vertexBuffer(_vertexBufferID)
		.texture(textureID,pixelShader,0)
		.samplerState(ssid,pixelShader)
		.pixelShader(pixelShader)
		.indexBuffer(indexBufferID)
		.build();

	return stateGroup;

}


ElasticBorder::~ElasticBorder() {
	for (int i = 0; i < 4; ++i) {
		delete[] _ribbons[i].rDeltas;
		delete[] _ribbons[i].lDeltas;
		delete[] _ribbons[i].points;
	}
}

// ---------------------------------------------------
// tick
// ---------------------------------------------------
void ElasticBorder::tick(float dt) {
	float delta = 1.0f / 60.0f;
	_timer += dt;
	if (_timer >= delta) {
		_timer -= delta;
		for (int i = 0; i < 4; ++i) {
			updateRibbon(&_ribbons[i]);
		}
	}
}

// ---------------------------------------------------
// render
// ---------------------------------------------------
void ElasticBorder::render() {
	ds::vec3 p[4];
	int idx = 0;
	for (int j = 0; j < 4; ++j) {
		const Ribbon& ribbon = _ribbons[j];
		for (int i = 0; i < ribbon.num - 1; ++i) {
			ds::Color fc = ribbon.points[i].color;
			fc.r = 192 + ribbon.points[i].height * 2.0f;
			fc.g = ribbon.points[i].height * 0.5f - 20.0f;
			ds::Color sc = ribbon.points[i + 1].color;
			sc.r = 192 + ribbon.points[i + 1].height * 2.0f;
			sc.g = ribbon.points[i + 1].height * 0.5f - 20.0f;
			if (ribbon.vertical) {
				ds::vec2 f = ds::vec2(0.0f, ribbon.points[i].height);				
				f += ribbon.points[i].pos;
				ds::vec2 s = ds::vec2(0.0f, ribbon.points[i + 1].height);
				s += ribbon.points[i + 1].pos;				
				_vertices[idx++] = { ds::vec3(f.x, f.y, 0.0f) , ds::vec2(40.0f,92.0f), fc };
				_vertices[idx++] = { ds::vec3(s.x, s.y, 0.0f) , ds::vec2(60.0f,92.0f), sc };
				_vertices[idx++] = { ds::vec3(s.x, s.y - _thickness, 0.0f) , ds::vec2(60.0f,112.0f), sc };
				_vertices[idx++] = { ds::vec3(f.x, f.y - _thickness, 0.0f) , ds::vec2(40.0f,112.0f), fc };
			}
			else {
				ds::vec2 f = ds::vec2(ribbon.points[i].height,0.0f);
				f += ribbon.points[i].pos;
				ds::vec2 s = ds::vec2(ribbon.points[i + 1].height,0.0f);
				s += ribbon.points[i + 1].pos;
				_vertices[idx++] = { ds::vec3(s.x - _thickness, s.y, 0.0f) , ds::vec2(260.0f,20.0f), sc };
				_vertices[idx++] = { ds::vec3(s.x, s.y, 0.0f) , ds::vec2(280.0f,20.0f), sc };
				_vertices[idx++] = { ds::vec3(f.x, f.y, 0.0f) , ds::vec2(280.0f,40.0f), fc };
				_vertices[idx++] = { ds::vec3(f.x - _thickness, f.y, 0.0f) , ds::vec2(260.0f,40.0f), fc };
			}
		}
	}
	// corner dots
	/*
	v3 dp[] = { v3(-10, 10, 0), v3(10, 10, 0), v3(10, -10, 0), v3(-10, -10, 0) };
	v3 cp[] = { v3(35, 35, 0), v3(35, 665, 0), v3(1235, 665, 0), v3(1235, 35, 0) };
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			p[j] = cp[i] + dp[j];
		}
		squares->draw(p, math::buildTexture(40, 120, 20, 20), ds::Color(43, 221, 237, 255));
	}
	*/
	ds::mapBufferData(_vertexBufferID, _vertices, idx * sizeof(GridVertex));
	ds::submit(_orthoPass, _drawItem, idx / 4 * 6);
}

bool ElasticBorder::collides(const ds::vec2& s, float r) {
	bool hit = false;
	if (s.x < 40.0f) {
		// left
		int idx = (s.y - 40.0f) / _length;
		splash(2, idx, 12.0f);
		hit = true;
	}
	if (s.y < 40.0f) {
		// bottom
		int idx = (s.x - 40.0f) / _length;
		splash(0, idx, 12.0f);
		hit = true;
	}
	if (s.y > 670.0f) {
		// top
		int idx = (s.x - 40.0f) / _length;
		splash(1, idx, 12.0f);
		hit = true;
	}
	if (s.x > 960.0f) {
		// right
		int idx = (s.y - 40.0f) / _length;
		splash(3, idx, 12.0f);
		hit = true;
	}
	return hit;
}

// ---------------------------------------------------
// splash
// ---------------------------------------------------
void ElasticBorder::splash(int ribbonIndex, int index, float speed) {
	Ribbon& ribbon = _ribbons[ribbonIndex];
	if (index >= 0 && index < ribbon.num) {
		ribbon.points[index].speed = speed;
	}
}


// ---------------------------------------------------
// splash
// ---------------------------------------------------
void ElasticBorder::splash(int index, float speed) {
	//if (index >= 0 && index < _total) {
		_ribbons[0].points[index].speed = speed;
		_ribbons[1].points[index].speed = speed;
		_ribbons[2].points[index].speed = speed;
		_ribbons[3].points[index].speed = speed;
	//}
}
