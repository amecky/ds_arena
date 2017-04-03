#include "ElasticBorder.h"
#include "..\shaders\Border_PS_Main.inc"
#include "..\shaders\Border_VS_Main.inc"
const float Tension = 0.025f;
const float Dampening = 0.025f;
const float Spread = 0.15f;

// ---------------------------------------------------
//
// AbstractBorder
//
// ---------------------------------------------------
ElasticBorder::ElasticBorder(float size, int numX, int numY, const ds::vec4& texture, RID textureID) //, const ds::AABBox& box) 
	: _size(size) , _numX(numX) , _numY(numY) , _texture(texture) , _textureID(textureID) {
	//_squareBuffer = ds::res::find("Squares", ds::ResourceType::SQUAREBUFFER);
	int total = numX * numY * 2;
	_points = new LinePoint[total];
	lDeltas = new float[total];
	rDeltas = new float[total];
	//for (int j = 0; j < 4; ++j) {
		for (int i = 0; i < _numX; ++i) {
			_points[i].height = 40.0f;
			_points[i].targetHeight = 40.0f;
			_points[i].speed = 0.0f;
			_points[i].dir = ds::vec2(0, 1);
		}
	//}

		_vertices = new GridVertex[total * 4];

		// create orthographic view
		ds::matrix orthoView = ds::matIdentity();
		ds::matrix orthoProjection = ds::matOrthoLH(ds::getScreenWidth(), ds::getScreenHeight(), 0.1f, 1.0f);
		_orthoPass = ds::createRenderPass(orthoView, orthoProjection, ds::DepthBufferState::DISABLED);

		_constantBuffer.viewprojectionMatrix = ds::matTranspose(orthoView * orthoProjection);
		_constantBuffer.worldMatrix = ds::matTranspose(ds::matIdentity());

		RID stateGroup = createStateGroup(total * 4);
		ds::DrawCommand drawCmd = { 100, ds::DrawType::DT_INDEXED, ds::PrimitiveTypes::TRIANGLE_LIST, 0 };

		_drawItem = ds::compile(drawCmd, stateGroup);

		_timer = 0.0f;
}

RID ElasticBorder::createStateGroup(int numVertices) {

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
		.pixelShader(pixelShader)
		.constantBuffer(cbid, vertexShader, 0)
		.vertexBuffer(_vertexBufferID)
		.indexBuffer(indexBufferID)
		.build();

	return stateGroup;

}


ElasticBorder::~ElasticBorder() {
	delete[] rDeltas;
	delete[] lDeltas;
	delete[] _points;
}

// ---------------------------------------------------
// tick
// ---------------------------------------------------
void ElasticBorder::tick(float dt) {
	float delta = 1.0f / 60.0f;
	_timer += dt;
	if (_timer >= delta) {
		_timer -= delta;
		//ZoneTracker("Borders::tick");
		for (int i = 1; i < _numX - 1; ++i) {
			LinePoint& lp = _points[i];
			float x = lp.targetHeight - lp.height;
			lp.speed += Tension * x - lp.speed * Dampening;
			lp.height += lp.speed;
		}

		// do some passes where columns pull on their neighbours
		for (int j = 0; j < 2; j++) {
			for (int i = 0; i < _numX; i++) {
				if (i > 0) {
					lDeltas[i] = Spread * (_points[i].height - _points[i - 1].height);
					_points[i - 1].speed += lDeltas[i];
				}
				if (i < _numX - 1) {
					rDeltas[i] = Spread * (_points[i].height - _points[i + 1].height);
					_points[i + 1].speed += rDeltas[i];
				}
			}

			for (int i = 0; i < _numX; i++) {
				if (i > 0) {
					_points[i - 1].height += lDeltas[i];
				}
				if (i < _numX - 1) {
					_points[i + 1].height += rDeltas[i];
				}
			}
		}
		// fix corner points
		_points[0].height = _points[0].targetHeight;
		_points[_numX - 1].height = _points[_numX - 1].targetHeight;
	}
}

// ---------------------------------------------------
// render
// ---------------------------------------------------
void ElasticBorder::render() {
	//ZoneTracker("Borders::render");
	ds::vec3 p[4];
	int idx = 0;
	for (int i = 0; i < _numX - 1; ++i) {

		ds::vec2 f = ds::vec2(30.0f + i * _size, _points[i].height);
		ds::vec2 s = ds::vec2(30.0f + (i + 1)* _size, _points[i + 1].height);
		_vertices[idx++] = { ds::vec3(f.x, f.y, 0.0f) , ds::vec2(520.0f,0.0f), ds::Color(255,255,255,255) };
		_vertices[idx++] = { ds::vec3(s.x, s.y, 0.0f) , ds::vec2(560.0f,0.0f), ds::Color(255,255,255,255) };
		_vertices[idx++] = { ds::vec3(s.x, s.y - 10.0f, 0.0f) , ds::vec2(560.0f,40.0f), ds::Color(255,255,255,255) };
		_vertices[idx++] = { ds::vec3(f.x, f.y - 10.0f, 0.0f) , ds::vec2(0.0f,40.0f), ds::Color(255,255,255,255) };

		//buildColumn(i, p);
		//squares->draw(p, _texture, ds::Color(43, 221, 237, 255));
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
	//int num = grid.mapData(vertices, NUM);

	ds::mapBufferData(_vertexBufferID, _vertices, idx * sizeof(GridVertex));

	ds::submit(_orthoPass, _drawItem, _numX * 6);
}

// ---------------------------------------------------
// splash
// ---------------------------------------------------
void ElasticBorder::splash(int index, float speed) {
	if (index >= 0 && index < _numX) {
		_points[index].speed = speed;
	}
}
/*
// ---------------------------------------------------
// is inside
// ---------------------------------------------------
bool ElasticBorder::isInside(const v3& pos) const {
	return _box.contains(pos);
}

bool ElasticBorder::collides(const ds::Sphere& s) const {
	return _box.overlaps(s);
}
*/
/*
// ---------------------------------------------------
//
// ElasticBorder
//
// ---------------------------------------------------
ElasticBorder::ElasticBorder() {
	_borders[0] = new BottomBorder(40.0f, 61);
	_borders[1] = new TopBorder(40.0f, 61);
	_borders[2] = new LeftBorder(40.0f, 33);
	_borders[3] = new RightBorder(40.0f, 33);
}

ElasticBorder::~ElasticBorder() {
	for (int i = 0; i < 4; ++i) {
		delete _borders[i];
	}
}

// ---------------------------------------------------
// tick
// ---------------------------------------------------
void ElasticBorder::tick(float dt) {
	for (int i = 0; i < 4; ++i) {
		_borders[i]->tick(dt);
	}
}

// ---------------------------------------------------
// render
// ---------------------------------------------------
void ElasticBorder::render() {
	for (int i = 0; i < 4; ++i) {
		_borders[i]->render();
	}
}

// ---------------------------------------------------
// splash
// ---------------------------------------------------
void ElasticBorder::splash(const v3& pos, float speed) {
	for (int i = 0; i < 4; ++i) {
		if (_borders[i]->isInside(pos)) {
			_borders[i]->splash(_borders[i]->getIndex(pos), speed);
		}
	}
}

void ElasticBorder::splash(const v3& pos, float radius, float speed) {
	ds::Sphere sp(pos, radius);
	for (int i = 0; i < 4; ++i) {
		if (_borders[i]->collides(sp)) {
			_borders[i]->splash(_borders[i]->getIndex(pos), speed);
		}
	}
}

// ---------------------------------------------------
//
// BottomBorder
//
// ---------------------------------------------------
BottomBorder::BottomBorder(float target, int num) 
	: AbstractBorder(target, num, math::buildTexture(0, 302, 36, 10), ds::AABBox(v3(640, 20, 0), v3(1280, 40, 0))) {
}

void BottomBorder::buildColumn(int index, v3* positions) {
	v2 f = v2(30.0f + index * 20.0f, _points[index].height);
	v2 s = v2(30.0f + (index + 1)* 20.0f, _points[index + 1].height);
	positions[0] = v3(f.x, f.y, 0.0f);
	positions[1] = v3(s.x, s.y, 0.0f);
	positions[2] = v3(s.x, s.y - 10.0f, 0.0f);
	positions[3] = v3(f.x, f.y - 10.0f, 0.0f);
}

int BottomBorder::getIndex(const v3& pos) const {
	return (pos.x - 20) / 20;
}

// ---------------------------------------------------
//
// TopBorder
//
// ---------------------------------------------------
TopBorder::TopBorder(float target, int num) 
	: AbstractBorder(target, num, math::buildTexture(30, 342, 36, 10), ds::AABBox(v3(640, 700, 0), v3(1280, 40, 0))) {
}

void TopBorder::buildColumn(int index, v3* positions) {
	v2 f = v2(30.0f + index * 20.0f, _points[index].height);
	v2 s = v2(30.0f + (index + 1)* 20.0f, _points[index + 1].height);
	positions[0] = v3(f.x, 700.0f - f.y + 10.0f, 0.0f);
	positions[1] = v3(s.x, 700.0f - s.y + 10.0f, 0.0f);
	positions[2] = v3(s.x, 700.0f - s.y, 0.0f);
	positions[3] = v3(f.x, 700.0f - f.y, 0.0f);
}

int TopBorder::getIndex(const v3& pos) const {
	return (pos.x - 20) / 20;
}

// ---------------------------------------------------
//
// LeftBorder
//
// ---------------------------------------------------
LeftBorder::LeftBorder(float target, int num) 
	: AbstractBorder(target, num, math::buildTexture(40, 330, 10, 36), ds::AABBox(v3(20, 360, 0), v3(20, 720, 0))) {
}

void LeftBorder::buildColumn(int index, v3* positions) {
	v2 f = v2(_points[index].height, 30.0f + index * 20.0f);
	v2 s = v2(_points[index + 1].height, 30.0f + (index + 1)* 20.0f);
	positions[0] = v3(s.x - 10.0f, s.y, 0.0f);
	positions[1] = v3(s.x, s.y, 0.0f);
	positions[2] = v3(f.x, f.y, 0.0f);
	positions[3] = v3(f.x - 10.0f, f.y, 0.0f);
}

int LeftBorder::getIndex(const v3& pos) const {
	return (pos.y - 20) / 20;
}

// ---------------------------------------------------
//
// RightBorder
//
// ---------------------------------------------------
RightBorder::RightBorder(float target, int num) 
	: AbstractBorder(target, num, math::buildTexture(40, 340, 10, 36), ds::AABBox(v3(1260, 360, 0), v3(20, 720, 0))) {
}

void RightBorder::buildColumn(int index, v3* positions) {
	v2 f = v2(_points[index].height, 30.0f + index * 20.0f);
	v2 s = v2(_points[index + 1].height, 30.0f + (index + 1)* 20.0f);
	positions[0] = v3(1270.0f - s.x, s.y, 0.0f);
	positions[1] = v3(1270.0f - s.x + 10.0f, s.y, 0.0f);
	positions[2] = v3(1270.0f - f.x + 10.0f, f.y, 0.0f);
	positions[3] = v3(1270.0f - f.x, f.y, 0.0f);
}

int RightBorder::getIndex(const v3& pos) const {
	return (pos.y - 20) / 20;
}
*/