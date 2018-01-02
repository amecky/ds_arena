#include "ElasticBorder.h"
#include "..\..\shaders\Border_PS_Main.inc"
#include "..\..\shaders\Border_VS_Main.inc"
#include "..\lib\math.h"
#include "..\utils\tweening.h"

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

void updateRibbon(Ribbon* ribbon, ElasticBorderSettings* settings) {	
	for (int i = 1; i < ribbon->num - 1; ++i) {
		LinePoint& lp = ribbon->points[i];
		float x = lp.targetHeight - lp.height;
		lp.speed += settings->Tension * x - lp.speed * settings->Dampening;
		lp.height += lp.speed;
	}

	// do some passes where columns pull on their neighbours
	for (int j = 0; j < 2; j++) {
		for (int i = 0; i < ribbon->num; i++) {
			if (i > 0) {
				ribbon->lDeltas[i] = settings->Spread * (ribbon->points[i].height - ribbon->points[i - 1].height);
				ribbon->points[i - 1].speed += ribbon->lDeltas[i];
			}
			if (i < ribbon->num - 1) {
				ribbon->rDeltas[i] = settings->Spread * (ribbon->points[i].height - ribbon->points[i + 1].height);
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
ElasticBorder::ElasticBorder(ElasticBorderSettings* settings) : _settings(settings) {

	
	float startX = (settings->world_size.x - _settings->numX * _settings->thickness) / 2;
	float startY = (settings->world_size.y - _settings->numY * _settings->thickness) / 2;
	float endX = startX + (_settings->numX - 1) * _settings->thickness;
	float endY = startY + (_settings->numY - 1) * _settings->thickness;

	_boundingBox = ds::vec4(startX, startY, endX, endY);
	// bottom
	initializeRibbon(&_ribbons[0], settings->numX, ds::vec2(startX, startY - _settings->thickness), ds::vec2(1, 0), true, settings->length);
	// top
	initializeRibbon(&_ribbons[1], settings->numX, ds::vec2(startX, endY - _settings->thickness), ds::vec2(1, 0), true, settings->length);
	// left
	initializeRibbon(&_ribbons[2], settings->numY, ds::vec2(startX - _settings->thickness, startY), ds::vec2(0, 1), false, settings->length);
	// right
	initializeRibbon(&_ribbons[3], settings->numY, ds::vec2(endX - _settings->thickness, startY), ds::vec2(0, 1), false, settings->length);

	// corner dots
	ds::vec3 p[4];
	int idx = 0;
	ds::vec3 BOX_VERTICES[] = { ds::vec3(-10, 10, 0), ds::vec3(10, 10, 0), ds::vec3(10, -10, 0), ds::vec3(-10, -10, 0) };
	ds::vec3 cp[] = { 
		ds::vec3(startX + _settings->thickness * 0.5f, startY + _settings->thickness * 0.5f, 0.0f),
		ds::vec3(startX + _settings->thickness * 0.5f, endY + _settings->thickness * 0.5f, 0.0f),
		ds::vec3(endX + _settings->thickness * 0.5f, endY + _settings->thickness * 0.5f, 0.0f),
		ds::vec3(endX + _settings->thickness * 0.5f, startY + _settings->thickness * 0.5f, 0.0f)
	};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			p[j] = cp[i] + BOX_VERTICES[j];
		}
		_cornerVertices[idx++] = { p[0] , ds::vec2(120.0f,40.0f), ds::Color(17, 112, 120, 255) };
		_cornerVertices[idx++] = { p[1] , ds::vec2(140.0f,40.0f), ds::Color(17, 112, 120, 255) };
		_cornerVertices[idx++] = { p[2] , ds::vec2(140.0f,60.0f), ds::Color(17, 112, 120, 255) };
		_cornerVertices[idx++] = { p[3] , ds::vec2(120.0f,60.0f), ds::Color(17, 112, 120, 255) };
	}

	int total = settings->numX * settings->numY * 2;
	_vertices = new GridVertex[total * 4];

	// create orthographic view
	ds::matrix orthoView = ds::matIdentity();
	ds::matrix orthoProjection = ds::matOrthoLH(ds::getScreenWidth(), ds::getScreenHeight(), 0.1f, 1.0f);
	_orthoCamera = {
		orthoView,
		orthoProjection,
		orthoView * orthoProjection,
		ds::vec3(0,0,0),
		ds::vec3(0,0,0),
		ds::vec3(0,1,0),
		ds::vec3(1,0,0),
		0.0f,
		0.0f,
		0.0f
	};

	ds::ViewportInfo vpInfo = { 1280, 720, 0.0f, 1.0f };
	RID vp = ds::createViewport(vpInfo);

	ds::RenderPassInfo rpInfo = { &_orthoCamera, vp, ds::DepthBufferState::DISABLED, 0, 0 };
	_orthoPass = ds::createRenderPass(rpInfo);

	_constantBuffer.viewprojectionMatrix = ds::matTranspose(orthoView * orthoProjection);
	_constantBuffer.worldMatrix = ds::matTranspose(ds::matIdentity());
	_constantBuffer.center = ds::vec4(640.0f, 360.0f, 0.0f, 1.0);
	RID stateGroup = createStateGroup(total * 4, settings->textureID);
	ds::DrawCommand drawCmd = { 100, ds::DrawType::DT_INDEXED, ds::PrimitiveTypes::TRIANGLE_LIST, 0 };

	_drawItem = ds::compile(drawCmd, stateGroup);

	_timer = 0.0f;
}

RID ElasticBorder::createStateGroup(int numVertices, RID textureID) {
	ds::BlendStateInfo myBlendState = { ds::BlendStates::SRC_ALPHA, ds::BlendStates::SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, true };
	RID bs_id = ds::createBlendState(myBlendState);

	ds::ShaderInfo vsInfo = { 0 , Border_VS_Main, sizeof(Border_VS_Main), ds::ShaderType::ST_VERTEX_SHADER };
	RID vertexShader = ds::createShader(vsInfo, "Border_VS");
	ds::ShaderInfo psInfo = { 0 , Border_PS_Main, sizeof(Border_PS_Main), ds::ShaderType::ST_PIXEL_SHADER };
	RID pixelShader = ds::createShader(psInfo, "Border_PS");

	ds::InputLayoutDefinition decl[] = {
		{ "POSITION", 0, ds::BufferAttributeType::FLOAT3 },
		{ "TEXCOORD", 0, ds::BufferAttributeType::FLOAT2 },
		{ "COLOR"   , 0, ds::BufferAttributeType::FLOAT4 }
	};

	int q = numVertices / 4 * 6;
	ds::InputLayoutInfo layoutInfo = { decl, 3, vertexShader };
	RID rid = ds::createInputLayout(layoutInfo);
	RID cbid = ds::createConstantBuffer(sizeof(CubeConstantBuffer), &_constantBuffer);
	RID indexBufferID = ds::createQuadIndexBuffer(numVertices / 4);
	ds::VertexBufferInfo vbInfo = { ds::BufferType::DYNAMIC, numVertices, sizeof(GridVertex) };
	_vertexBufferID = ds::createVertexBuffer(vbInfo);
	ds::SamplerStateInfo samplerInfo = { ds::TextureAddressModes::CLAMP, ds::TextureFilters::LINEAR };
	RID ssid = ds::createSamplerState(samplerInfo);


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
			updateRibbon(&_ribbons[i], _settings);
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

			ds::Color clr = ds::Color(35, 230, 247, 255);
			fc = clr;
			sc = clr;
			float d = fabs(static_cast<float>(ribbon.points[i].targetHeight - ribbon.points[i].height) / _settings->length);
			if (d > 0.1f) {
				fc = tweening::interpolate(tweening::easeSinus, ds::Color(35, 230, 247, 255), ds::Color(17, 112, 120,255), d, 1.0f);
			}
			d = fabs(static_cast<float>(ribbon.points[i + 1].targetHeight - ribbon.points[i + 1].height) / _settings->length);
			if (d > 0.1f) {
				sc = tweening::interpolate(tweening::easeSinus, ds::Color(35, 230, 247, 255), ds::Color(17, 112, 120, 255), d, 1.0f);
			}

			if (ribbon.vertical) {
				ds::vec2 f = ds::vec2(0.0f, ribbon.points[i].height);				
				f += ribbon.points[i].pos;
				ds::vec2 s = ds::vec2(0.0f, ribbon.points[i + 1].height);
				s += ribbon.points[i + 1].pos;				
				_vertices[idx++] = { ds::vec3(f.x, f.y, 0.0f) , ds::vec2(40.0f,92.0f), fc };
				_vertices[idx++] = { ds::vec3(s.x, s.y, 0.0f) , ds::vec2(60.0f,92.0f), sc };
				_vertices[idx++] = { ds::vec3(s.x, s.y - _settings->thickness, 0.0f) , ds::vec2(60.0f,112.0f), sc };
				_vertices[idx++] = { ds::vec3(f.x, f.y - _settings->thickness, 0.0f) , ds::vec2(40.0f,112.0f), fc };
			}
			else {
				ds::vec2 f = ds::vec2(ribbon.points[i].height,0.0f);
				f += ribbon.points[i].pos;
				ds::vec2 s = ds::vec2(ribbon.points[i + 1].height,0.0f);
				s += ribbon.points[i + 1].pos;
				_vertices[idx++] = { ds::vec3(s.x - _settings->thickness, s.y, 0.0f) , ds::vec2(260.0f,20.0f), sc };
				_vertices[idx++] = { ds::vec3(s.x, s.y, 0.0f) , ds::vec2(280.0f,20.0f), sc };
				_vertices[idx++] = { ds::vec3(f.x, f.y, 0.0f) , ds::vec2(280.0f,40.0f), fc };
				_vertices[idx++] = { ds::vec3(f.x - _settings->thickness, f.y, 0.0f) , ds::vec2(260.0f,40.0f), fc };
			}
		}
	}
	
	// corner dots
	for (int i = 0; i < 16; ++i) {
		_vertices[idx++] = _cornerVertices[i];
	}
	

	ds::mapBufferData(_vertexBufferID, _vertices, idx * sizeof(GridVertex));
	ds::submit(_orthoPass, _drawItem, idx / 4 * 6);
}

bool ElasticBorder::collides(const ds::vec2& s, float r) {
	bool hit = false;
	int sides = math::out_of_bounds(s, _boundingBox);

	if ((sides & 1) == 1) {
		// left
		int idx = (s.y - 40.0f) / _settings->length;
		splash(2, idx, _settings->splashForce);
		hit = true;
	}
	if ((sides & 8) == 8) {
		// bottom
		int idx = (s.x - 40.0f) / _settings->length;
		splash(0, idx, _settings->splashForce);
		hit = true;
	}
	if ((sides & 2) == 2) {
		// top
		int idx = (s.x - 40.0f) / _settings->length;
		splash(1, idx, _settings->splashForce);
		hit = true;
	}
	if ((sides & 4) == 4) {
		// right
		int idx = (s.y - 40.0f) / _settings->length;
		splash(3, idx, _settings->splashForce);
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

void ElasticBorder::setScreenCenter(const ds::vec2& center) {
	_constantBuffer.center.x = center.x;
	_constantBuffer.center.y = center.y;
}
