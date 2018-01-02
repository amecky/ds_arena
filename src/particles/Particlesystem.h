#pragma once
#include <stdint.h>
#include <diesel.h>
#include <vector>

struct GPUParticle {
	ds::vec2 position;
	float rotation;
	ds::vec2 velocity;
	ds::vec2 acceleration;
	ds::vec2 timer;
	ds::vec2 scale;
	ds::vec2 growth;
	ds::Color color;
	float decay;
	ds::vec4 texRect;
};

// ---------------------------------------------------------------
// the sprite constant buffer
// ---------------------------------------------------------------
struct ParticleConstantBuffer {
	ds::vec4 screenCenter;
	ds::matrix wvp;
};

// -------------------------------------------------------
// Particle array
// -------------------------------------------------------
struct ParticleArray {

	ds::vec2* positions;
	ds::vec2* velocities;
	ds::vec2* accelerations;
	float* rotations;
	ds::vec2* scales;
	ds::vec2* growth;
	ds::vec3* timers;
	ds::Color* colors;
	float* decays;
	ds::vec4* texRects;
	char* buffer;

	uint32_t count;
	uint32_t countAlive;

	ParticleArray() : count(0), countAlive(0), buffer(0) {}

	~ParticleArray() {
		if (buffer != 0) {
			delete[] buffer;
		}
	}

	void initialize(unsigned int maxParticles) {
		int size = maxParticles * (sizeof(ds::vec2) + sizeof(ds::vec2) + sizeof(ds::vec2) + sizeof(float) + sizeof(ds::vec2) + sizeof(ds::vec2) + sizeof(ds::vec3) + sizeof(ds::Color) + sizeof(float) + sizeof(ds::vec4));
		buffer = new char[size];
		positions = (ds::vec2*)(buffer);
		velocities = (ds::vec2*)(positions + maxParticles);
		accelerations = (ds::vec2*)(velocities + maxParticles);
		rotations = (float*)(accelerations + maxParticles);
		scales = (ds::vec2*)(rotations + maxParticles);
		growth = (ds::vec2*)(scales + maxParticles);
		timers = (ds::vec3*)(growth + maxParticles);
		colors = (ds::Color*)(timers + maxParticles);
		decays = (float*)(colors + maxParticles);
		texRects = (ds::vec4*)(decays + maxParticles);
		count = maxParticles;
		countAlive = 0;
	}

	void swapData(uint32_t a, uint32_t b) {
		if (a != b) {
			positions[a] = positions[b];
			velocities[a] = velocities[b];
			accelerations[a] = accelerations[b];
			rotations[a] = rotations[b];
			scales[a] = scales[b];
			growth[a] = growth[b];
			timers[a] = timers[b];
			colors[a] = colors[b];
			decays[a] = decays[b];
			texRects[a] = texRects[b];
		}
	}

	void kill(uint32_t id) {
		if (countAlive > 0) {
			swapData(id, countAlive - 1);
			--countAlive;
		}
	}

	void wake(uint32_t id) {
		if (countAlive < count) {
			++countAlive;
		}
	}

};

// -------------------------------------------------------
// Particlesystem descriptor
// -------------------------------------------------------
struct ParticlesystemDescriptor {
	uint16_t maxParticles;
	RID textureID;
};

// -------------------------------------------------------
// Particle emitter settings
// -------------------------------------------------------
struct EmitterSettings {
	int count;
	ds::vec2 ttl;
	float angleVariance;
	float radius;
	float radiusVariance;
	ds::vec2 velocity;
	ds::vec2 velocityVariance;
	ds::vec2 size;
	ds::vec2 sizeVariance;
	ds::vec2 growth;
	ds::vec2 acceleration;
	ds::Color color;
	float decay;
	ds::vec4 texRect;
};

// -------------------------------------------------------
// Particlesystem
// -------------------------------------------------------
struct Particlesystem {
	int maxParticles;
	RID textureID;
	ParticleArray array;
};

// -------------------------------------------------------
// ParticlesystemRenderer
// -------------------------------------------------------
class ParticleManager {

public:
	ParticleManager(int maxParticles, RID textureID);
	~ParticleManager();
	uint32_t add(const ParticlesystemDescriptor& descriptor);
	void emitt(uint32_t id, const ds::vec2& pos, const EmitterSettings& emitter);
	void tick(float dt);
	void render();
	int numAlive(uint32_t id) const {
		return _particleSystems[id]->array.countAlive;
	}
	void setScreenCenter(const ds::vec2& center);
private:
	ParticleConstantBuffer _constantBuffer;
	std::vector<Particlesystem*> _particleSystems;
	GPUParticle* _vertices;
	ds::matrix _viewprojectionMatrix;
	RID _drawItem;
	RID _orthoPass;
	RID _structuredBufferId;
};