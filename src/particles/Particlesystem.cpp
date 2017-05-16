#include "Particlesystem.h"
#include "Particles_GS_Main.inc"
#include "Particles_VS_Main.inc"
#include "Particles_PS_Main.inc"
#include "..\utils\json.h"
// ------------------------------------------------------ -
// create new particlesystem
// -------------------------------------------------------
ParticleManager::ParticleManager(int maxParticles, RID textureID) {
	_vertices = new ParticleVertex[maxParticles];
	_constantBuffer.screenDimension = ds::vec4(1024.0f, 768.0f, 1024.0f, 1024.0f);
	_constantBuffer.screenCenter = ds::vec4(512.0f, 384.0f, 0.0f, 0.0f);
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
	ds::RenderPassInfo rpInfo = { &_orthoCamera, ds::DepthBufferState::DISABLED, 0, 0 };
	_orthoPass = ds::createRenderPass(rpInfo, "ParticleOrthoPass");

	ds::ShaderInfo vsInfo = { 0 , Particles_VS_Main, sizeof(Particles_VS_Main), ds::ShaderType::ST_VERTEX_SHADER };
	RID vertexShader = ds::createShader(vsInfo, "ParticlesVS");
	ds::ShaderInfo psInfo = { 0 , Particles_PS_Main, sizeof(Particles_PS_Main), ds::ShaderType::ST_PIXEL_SHADER };
	RID pixelShader = ds::createShader(psInfo, "ParticlesPS");	
	ds::ShaderInfo gsInfo = { 0 , Particles_GS_Main, sizeof(Particles_GS_Main), ds::ShaderType::ST_GEOMETRY_SHADER };
	RID geoShader = ds::createShader(gsInfo, "ParticlesGS");

	// very special buffer layout 
	ds::InputLayoutDefinition decl[] = {
		{ ds::BufferAttribute::POSITION, ds::BufferAttributeType::FLOAT,3 },
		{ ds::BufferAttribute::NORMAL, ds::BufferAttributeType::FLOAT,3 },
		{ ds::BufferAttribute::TANGENT ,ds::BufferAttributeType::FLOAT,3 },
		{ ds::BufferAttribute::COLOR, ds::BufferAttributeType::FLOAT,4 }
	};
	ds::InputLayoutInfo layoutInfo = { decl, 4, vertexShader };
	RID vertexDeclaration = ds::createInputLayout(layoutInfo, "ParticleLayout");

	ds::BlendStateInfo psBlendState = { ds::BlendStates::SRC_ALPHA, ds::BlendStates::ONE, ds::BlendStates::ONE, ds::BlendStates::ONE, true };
	RID bs_id = ds::createBlendState(psBlendState);
	RID constantBuffer = ds::createConstantBuffer(sizeof(ParticleConstantBuffer), &_constantBuffer);
	ds::VertexBufferInfo vbInfo = { ds::BufferType::DYNAMIC, maxParticles, sizeof(ParticleVertex) };
	_vertexBuffer = ds::createVertexBuffer(vbInfo);

	RID basicGroup = ds::StateGroupBuilder()
		.inputLayout(vertexDeclaration)
		.vertexShader(vertexShader)
		//.blendState(bs_id)
		.geometryShader(geoShader)
		.pixelShader(pixelShader)
		.constantBuffer(constantBuffer, geoShader, 0)
		.texture(textureID, pixelShader, 0)
		.vertexBuffer(_vertexBuffer)
		.build();

	ds::DrawCommand drawCmd = { 100, ds::DrawType::DT_VERTICES, ds::PrimitiveTypes::POINT_LIST };

	_drawItem = ds::compile(drawCmd, basicGroup, "ParticleDrawItem");

}

ParticleManager::~ParticleManager() {
	for (size_t i = 0; i < _systems.size(); ++i) {
		delete _systems[i];
	}
}

static void loadSystem(const SJSONReader& reader, const char* catName, ParticlesystemDescriptor* descriptor) {
	reader.get("max_particles", &descriptor->maxParticles, catName);
	reader.get("particle_dimension", &descriptor->particleDimension, catName);
	reader.get("scale", &descriptor->scale, catName);
	reader.get("growth", &descriptor->growth, catName);
	reader.get("start_color", &descriptor->startColor, catName);
	reader.get("end_color", &descriptor->endColor, catName);
	reader.get("texture_rect", &descriptor->textureRect, catName);
}

PSUID ParticleManager::load(const char* categoryName, RID textureID) {
	SJSONReader reader;
	reader.parse("content\\particlesystems.json");
	ParticlesystemDescriptor descriptor;
	loadSystem(reader, categoryName, &descriptor);
	descriptor.textureID = textureID;
	Particlesystem* system = new Particlesystem(descriptor);
	_systems.push_back(system);
	return _systems.size() - 1;
}

void ParticleManager::tick(float dt) {
	for (size_t p = 0; p < _systems.size(); ++p) {
		_systems[p]->tick(dt);
	}
}

void ParticleManager::render() {
	ds::matrix w = ds::matIdentity();
	_constantBuffer.wvp = ds::matTranspose(_orthoCamera.viewProjectionMatrix);
	for (size_t p = 0; p < _systems.size(); ++p) {
		_systems[p]->preapreBuffer(&_constantBuffer);
		const ParticleArray* array = _systems[p]->getArray();
		const ParticlesystemDescriptor& desc = _systems[p]->getDescriptor();
		for (int i = 0; i < array->countAlive; ++i) {
			_vertices[i] = { array->positions[i], array->velocities[i], array->accelerations[i], ds::vec4(array->timers[i].x, array->timers[i].y, 0.0f, array->rotations[i]) };
		}
		ds::mapBufferData(_vertexBuffer, _vertices, array->countAlive * sizeof(ParticleVertex));
		ds::submit(_orthoPass, _drawItem, array->countAlive);
	}
}

void ParticleManager::emitt(PSUID systemID, const ParticlesystemInstanceSettings& settings, float px, float py, float radius) {
	Particlesystem* system = _systems[systemID];
	for (int i = 0; i < settings.count; ++i) {
		float angle = ds::TWO_PI * static_cast<float>(i) / static_cast<float>(settings.count);
		float x = px + cos(angle) * (radius + ds::random(-settings.radiusVariance, settings.radiusVariance));
		float y = py + sin(angle) * (radius + ds::random(-settings.radiusVariance, settings.radiusVariance));
		float da = angle * settings.angleVariance;
		angle += ds::random(-da, da);
		ds::vec2 s = ds::vec2(0.2f, 0.15f);
		float ds = ds::random(settings.sizeVariance.x, settings.sizeVariance.y);
		s *= ds;
		float ttl = ds::random(settings.ttl.x, settings.ttl.y);
		float rotation = angle;
		ds::vec2 velocity = ds::random(settings.velocityVariance.x, settings.velocityVariance.y) * ds::vec2(cos(angle), sin(angle));
		ds::vec2 acc = velocity * ds::random(settings.accelerationVariance.x, settings.accelerationVariance.y);
		system->add(ds::vec2(x, y), velocity, acc, ttl, rotation);
	}
}

// -------------------------------------------------------
// create new particlesystem
// -------------------------------------------------------
Particlesystem::Particlesystem(ParticlesystemDescriptor descriptor) : _descriptor(descriptor) {
	_array.initialize(descriptor.maxParticles);
}

// -------------------------------------------------------
// add particle based on ParticleDescriptor
// -------------------------------------------------------
void Particlesystem::add(const ds::vec2& pos, const ds::vec2& velocity, const ds::vec2& acceleration, float ttl, float rotation) {
	int start = _array.countAlive;
	if ((start + 1) < _array.count) {	
		_array.rotations[start] = rotation;
		_array.timers[start] = ds::vec3(0.0f, ttl, 1);
		_array.velocities[start] = ds::vec3(velocity);
		_array.positions[start] = ds::vec3(pos);
		_array.accelerations[start] = ds::vec3(acceleration);
		_array.wake(start);
	}
}

// -------------------------------------------------------
// tick - updates all particles and manages lifetime
// -------------------------------------------------------
void Particlesystem::tick(float dt) {
	for (int i = 0; i < _array.countAlive; ++i) {
		ds::vec3 t = _array.timers[i];
		t.x += dt;
		t.z = t.x / t.y;
		_array.timers[i] = t;
		if (t.z >= 1.0f) {
			_array.kill(i);
		}
	}
}
