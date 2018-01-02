#pragma once
#include <diesel.h>
#include <SpriteBatchBuffer.h>

typedef uint32_t SUID;
const uint32_t INVALID_SUID = UINT32_MAX;

struct SpriteArrayIndex {
	SUID id;
	uint16_t index;
};

struct SpriteType {

	enum Enum {
		PLAYER, BULLET, ENEMY, DUMMY
	};
};

struct SpriteArray {

	uint16_t num;
	uint16_t capacity;
	SpriteArrayIndex* indices;
	SUID* ids;
	ds::vec2* positions;
	ds::vec2* scales;
	float* rotations;
	ds::vec4* textures;
	ds::Color* colors;
	SpriteType::Enum* types;
	int *energies;
	// physical
	ds::vec2* velocities;
	ds::vec2* forces;
	ds::vec2* previous;
	float* extents;
	char* buffer;

	SUID current;
	SUID* freeList;
	uint32_t num_free;
};

namespace sprites {

	void initialize(SpriteArray& array, int size);

	void clear(SpriteArray& array);

	bool contains(SpriteArray& array, SUID sid);

	uint32_t get_index(SpriteArray& array, SUID sid);

	ds::vec2 get_position(SpriteArray& array, SUID sid);

	int get_energy(SpriteArray& array, SUID sid);

	void set_energy(SpriteArray& array, SUID sid, int energy);

	SUID create(SpriteArray& array, const ds::vec2& pos, const ds::vec4& r, SpriteType::Enum type, int energy = 0, float rotation = 0.0f, float scaleX = 1.0f, float scaleY = 1.0f, const ds::Color& color = ds::Color(255,255,255,255));

	void remove(SpriteArray& array, SUID id);

	void render(SpriteArray& array, SpriteBatchBuffer* buffer);
}