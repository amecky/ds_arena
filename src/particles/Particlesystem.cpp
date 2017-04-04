#include "Particlesystem.h"
#include "Particles_GS_Main.inc"
#include "Particles_VS_Main.inc"
#include "Particles_PS_Main.inc"
// ------------------------------------------------------ -
// create new particlesystem
// -------------------------------------------------------
ParticleManager::ParticleManager(int maxParticles, RID textureID) {
	_vertices = new ParticleVertex[maxParticles];
	_constantBuffer.screenDimension = ds::vec4(1024.0f, 768.0f, 64.0f, 64.0f);
	_constantBuffer.screenCenter = ds::vec4(512.0f, 384.0f, 0.0f, 0.0f);
	ds::matrix viewMatrix = ds::matIdentity();
	ds::matrix projectionMatrix = ds::matOrthoLH(1024.0f, 768.0f, 0.1f, 1.0f);
	_viewprojectionMatrix = viewMatrix * projectionMatrix;

	RID vertexShader = ds::createVertexShader(Particles_VS_Main, sizeof(Particles_VS_Main), "ParticlesVS");
	RID pixelShader = ds::createPixelShader(Particles_PS_Main, sizeof(Particles_PS_Main), "ParticlesPS");
	RID geoShader = ds::createGeometryShader(Particles_GS_Main, sizeof(Particles_GS_Main), "ParticlesGS");

	// very special buffer layout 
	ds::VertexDeclaration decl[] = {
		{ ds::BufferAttribute::POSITION, ds::BufferAttributeType::FLOAT,3 },
		{ ds::BufferAttribute::NORMAL, ds::BufferAttributeType::FLOAT,3 },
		{ ds::BufferAttribute::TANGENT ,ds::BufferAttributeType::FLOAT,3 },
		{ ds::BufferAttribute::COLOR, ds::BufferAttributeType::FLOAT,4 }
	};
	RID vertexDeclaration = ds::createVertexDeclaration(decl, 4, vertexShader, "ParticleLayout");

	RID bs_id = ds::createBlendState(ds::BlendStates::SRC_ALPHA, ds::BlendStates::ONE, ds::BlendStates::ONE, ds::BlendStates::ONE, true);
	RID constantBuffer = ds::createConstantBuffer(sizeof(ParticleConstantBuffer), &_constantBuffer);
	_vertexBuffer = ds::createVertexBuffer(ds::BufferType::DYNAMIC, maxParticles, sizeof(ParticleVertex));

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

	// create orthographic view
	ds::matrix orthoView = ds::matIdentity();
	ds::matrix orthoProjection = ds::matOrthoLH(ds::getScreenWidth(), ds::getScreenHeight(), 0.1f, 1.0f);
	_orthoPass = ds::createRenderPass(orthoView, orthoProjection, ds::DepthBufferState::DISABLED, "ParticleOrthoPass");
	//constantBuffer.wvp = ds::matTranspose(orthoView * orthoProjection);

}

void ParticleManager::add(Particlesystem* system) {
	_systems.push_back(system);
}

void ParticleManager::tick(float dt) {
	for (size_t p = 0; p < _systems.size(); ++p) {
		_systems[p]->tick(dt);
	}
}

void ParticleManager::render() {
	ds::matrix w = ds::matIdentity();
	_constantBuffer.wvp = ds::matTranspose(_viewprojectionMatrix);
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
		//_array.accelerations[start] = ds::vec3(velocity) * -0.8f;
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