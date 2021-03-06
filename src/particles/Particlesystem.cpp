#include "Particlesystem.h"
#include "Particles_VS_Main.h"
#include "Particles_PS_Main.h"
// ------------------------------------------------------ -
// create new particlesystem
// -------------------------------------------------------
ParticleManager::ParticleManager(int maxParticles, RID textureID) {
	_vertices = new GPUParticle[maxParticles];
	float sw = ds::getScreenWidth();
	float sh = ds::getScreenHeight();
	_constantBuffer.screenCenter = ds::vec4(sw * 0.5f, sh * 0.5f, 0.0f, 0.0f);
	ds::matrix viewMatrix = ds::matIdentity();
	ds::matrix projectionMatrix = ds::matOrthoLH(sw, sh, 0.1f, 1.0f);
	_viewprojectionMatrix = viewMatrix * projectionMatrix;

	ds::ShaderInfo vsInfo = { 0 , Particles_VS_Main, sizeof(Particles_VS_Main), ds::ShaderType::ST_VERTEX_SHADER};
	RID vertexShader = ds::createShader(vsInfo, "ParticlesVS");
	ds::ShaderInfo psInfo = { 0 , Particles_PS_Main, sizeof(Particles_PS_Main), ds::ShaderType::ST_PIXEL_SHADER };
	RID pixelShader = ds::createShader(psInfo, "ParticlesPS");

	ds::BlendStateInfo blendInfo = { ds::BlendStates::SRC_ALPHA, ds::BlendStates::ZERO, ds::BlendStates::ONE, ds::BlendStates::ZERO, true };
	RID bs_id = ds::createBlendState(blendInfo);

	RID constantBuffer = ds::createConstantBuffer(sizeof(ParticleConstantBuffer), &_constantBuffer);

	ds::SamplerStateInfo samplerInfo = { ds::TextureAddressModes::CLAMP, ds::TextureFilters::LINEAR };
	RID ssid = ds::createSamplerState(samplerInfo);

	int indices[] = { 0,1,2,1,3,2 };
	RID idxBuffer = ds::createQuadIndexBuffer(maxParticles, indices);

	ds::StructuredBufferInfo sbInfo;
	sbInfo.cpuWritable = true;
	sbInfo.data = 0;
	sbInfo.elementSize = sizeof(GPUParticle);
	sbInfo.numElements = maxParticles;
	sbInfo.gpuWritable = false;
	sbInfo.renderTarget = NO_RID;
	sbInfo.textureID = NO_RID;
	_structuredBufferId = ds::createStructuredBuffer(sbInfo);

	RID basicGroup = ds::StateGroupBuilder()
		.constantBuffer(constantBuffer, vertexShader)
		.structuredBuffer(_structuredBufferId, vertexShader, 1)
		.vertexBuffer(NO_RID)
		.vertexShader(vertexShader)
		.indexBuffer(idxBuffer)
		.pixelShader(pixelShader)
		.samplerState(ssid, pixelShader)
		.blendState(bs_id)
		.texture(textureID, pixelShader, 0)
		.build();

	ds::DrawCommand drawCmd = { 100, ds::DrawType::DT_INDEXED, ds::PrimitiveTypes::TRIANGLE_LIST };

	_drawItem = ds::compile(drawCmd, basicGroup, "ParticleDrawItem");

	// create orthographic view
	ds::matrix orthoView = ds::matIdentity();
	ds::matrix orthoProjection = ds::matOrthoLH(ds::getScreenWidth(), ds::getScreenHeight(), 0.1f, 1.0f);
	ds::Camera camera = {
		orthoView,
		orthoProjection,
		orthoView * orthoProjection,
		ds::vec3(0,3,-6),
		ds::vec3(0,0,1),
		ds::vec3(0,1,0),
		ds::vec3(1,0,0),
		0.0f,
		0.0f,
		0.0f
	};

	ds::ViewportInfo vpInfo = { 1280, 720, 0.0f, 1.0f };
	RID vp = ds::createViewport(vpInfo);

	ds::RenderPassInfo rpInfo = { &camera, vp, ds::DepthBufferState::DISABLED, 0, 0 };
	_orthoPass = ds::createRenderPass(rpInfo, "ParticleOrthoPass");

}

ParticleManager::~ParticleManager() {
	// always clean up
	std::vector<Particlesystem*>::iterator it = _particleSystems.begin();
	while (it != _particleSystems.end()) {
		delete (*it);
		it = _particleSystems.erase(it);
	}
	delete[] _vertices;
}

uint32_t ParticleManager::add(const ParticlesystemDescriptor& descriptor) {
	Particlesystem* ps = new Particlesystem;
	ps->textureID = descriptor.textureID;
	ps->maxParticles = descriptor.maxParticles;
	ps->array.initialize(descriptor.maxParticles);
	_particleSystems.push_back(ps);
	return _particleSystems.size() - 1;
}

void ParticleManager::tick(float dt) {
	for (size_t p = 0; p < _particleSystems.size(); ++p) {
		Particlesystem* ps = _particleSystems[p];
		for (int i = 0; i < ps->array.countAlive; ++i) {
			ds::vec3 t = ps->array.timers[i];
			t.x += dt;
			t.z = t.x / t.y;
			ps->array.timers[i] = t;
			if (t.z >= 1.0f) {
				ps->array.kill(i);
			}
		}
	}
}

void ParticleManager::render() {
	ds::matrix w = ds::matIdentity();
	_constantBuffer.wvp = ds::matTranspose(_viewprojectionMatrix);
	for (size_t p = 0; p < _particleSystems.size(); ++p) {
		const ParticleArray& array = _particleSystems[p]->array;
		if (array.countAlive > 0) {
			for (int i = 0; i < array.countAlive; ++i) {
				_vertices[i] = {
					array.positions[i],
					array.rotations[i],
					array.velocities[i],
					array.accelerations[i],
					ds::vec2(array.timers[i].x,array.timers[i].z),
					array.scales[i],
					array.growth[i],
					array.colors[i],
					array.decays[i],
					array.texRects[i]
				};
			}
			ds::mapBufferData(_structuredBufferId, _vertices, array.countAlive * sizeof(GPUParticle));
			ds::submit(_orthoPass, _drawItem, array.countAlive * 6);
		}
	}
}

void ParticleManager::setScreenCenter(const ds::vec2& center) {
	_constantBuffer.screenCenter.x = center.x;
	_constantBuffer.screenCenter.y = center.y;
}

// -------------------------------------------------------
// add particle based on ParticleDescriptor
// -------------------------------------------------------
void ParticleManager::emitt(uint32_t id, const ds::vec2& pos, const EmitterSettings& emitter) {
	ParticleArray& array = _particleSystems[id]->array;
	int start = array.countAlive;
	for (int i = 0; i < emitter.count; ++i) {
		if ((start + 1) < array.count) {
			float angle = ds::TWO_PI * static_cast<float>(i) / static_cast<float>(emitter.count);
			float x = pos.x + cos(angle) * (emitter.radius + ds::random(-emitter.radiusVariance, emitter.radiusVariance));
			float y = pos.y + sin(angle) * (emitter.radius + ds::random(-emitter.radiusVariance, emitter.radiusVariance));
			float da = angle * emitter.angleVariance;
			angle += ds::random(-da, da);
			array.rotations[start] = angle;
			ds::vec2 s(1.0f);
			s.x = emitter.size.x + ds::random(-emitter.sizeVariance.x, emitter.sizeVariance.x);
			if (s.x <= 0.0f) {
				s.x = 0.1f;
			}
			s.y = emitter.size.y + ds::random(-emitter.sizeVariance.y, emitter.sizeVariance.y);
			if (s.y <= 0.0f) {
				s.y = 0.1f;
			}
			array.scales[start] = s;
			array.growth[start] = emitter.growth;
			float ttl = ds::random(emitter.ttl.x, emitter.ttl.y);
			array.timers[start] = ds::vec3(0.0f, ttl, 1.0f);
			array.velocities[start] = ds::random(emitter.velocityVariance.x, emitter.velocityVariance.y) * ds::vec2(cos(angle), sin(angle));
			array.accelerations[start].x = array.velocities[start].x * emitter.acceleration.x;
			array.accelerations[start].y = array.velocities[start].y * emitter.acceleration.y;
			array.positions[start] = ds::vec2(x,y);
			array.colors[start] = emitter.color;
			array.decays[start] = emitter.decay;
			array.texRects[start] = emitter.texRect;
			array.wake(start);
			++start;
		}
	}
}


