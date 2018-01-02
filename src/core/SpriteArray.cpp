#include "SpriteArray.h"
#include <assert.h>

namespace sprites {

	void static allocate(SpriteArray& array, uint16_t size) {
		int sz = size * (sizeof(SpriteArrayIndex) + sizeof(SUID) + 2 * sizeof(ds::vec2) + sizeof(float) + sizeof(ds::vec4) + sizeof(ds::Color) + sizeof(SpriteType::Enum) + sizeof(int) + 3 * sizeof(ds::vec2) + sizeof(float));
		char* b = new char[sz];
		array.capacity = size;
		array.indices = (SpriteArrayIndex*)b;
		array.ids = (SUID*)(array.indices + size);
		array.positions = (ds::vec2*)(array.ids + size);
		array.scales = (ds::vec2*)(array.positions + size);
		array.rotations = (float*)(array.scales + size);
		array.textures = (ds::vec4*)(array.rotations + size);
		array.colors = (ds::Color*)(array.textures + size);
		array.types = (SpriteType::Enum*)(array.colors + size);
		array.energies = (int*)(array.types + size);
		array.velocities = (ds::vec2*)(array.energies + size);
		array.forces = (ds::vec2*)(array.velocities + size);
		array.previous = (ds::vec2*)(array.forces + size);
		array.extents = (float*)(array.previous + size);
		array.buffer = b;
		array.freeList = new SUID[size];
	}

	void clear(SpriteArray& array) {
		if (array.buffer != 0) {
			for (unsigned short i = 0; i < array.capacity; ++i) {
				array.indices[i].id = i;
				array.indices[i].index = UINT16_MAX;
			}
			array.num = 0;
			array.current = 0;
			array.num_free = 0;
		}
	}

	void initialize(SpriteArray& array, int size) {
		allocate(array, size);
		clear(array);
	}

	bool contains(SpriteArray& array, SUID sid) {
		return false;
	}

	uint32_t get_index(SpriteArray& array, SUID sid) {
		SpriteArrayIndex &in = array.indices[sid];
		//assertSID(sid);
		return in.index;
	}

	ds::vec2 get_position(SpriteArray& array, SUID sid) {
		SpriteArrayIndex &in = array.indices[sid];
		return array.positions[in.index];
	}

	int get_energy(SpriteArray& array, SUID sid) {
		SpriteArrayIndex &in = array.indices[sid];
		return array.energies[in.index];
	}

	void set_energy(SpriteArray& array, SUID sid, int energy) {
		SpriteArrayIndex &in = array.indices[sid];
		array.energies[in.index] = energy;
	}

	SUID create(SpriteArray& array, const ds::vec2& pos, const ds::vec4& r, SpriteType::Enum type, int energy, float rotation, float scaleX, float scaleY, const ds::Color& color) {
		if ((array.num + 1) < array.capacity) {
			SUID id = 0;
			if (array.num_free == 0) {
				id = array.current++;
				//ds::log(LogLevel::LL_DEBUG, "1> creating new id %d", id);
			}
			else {
				id = array.freeList[--array.num_free];
				//ds::log(LogLevel::LL_DEBUG,"2> using id %d - free: %d", id, array.num_free);
			}
			SpriteArrayIndex &in = array.indices[id];
			in.index = array.num++;
			array.ids[in.index] = in.id;
			array.positions[in.index] = pos;
			array.scales[in.index] = ds::vec2(scaleX, scaleY);
			array.rotations[in.index] = rotation;
			array.textures[in.index] = r;
			array.colors[in.index] = color;
			array.types[in.index] = type;
			array.velocities[in.index] = ds::vec2(0.0f);
			array.forces[in.index] = ds::vec2(0.0f);
			array.previous[in.index] = pos;
			array.extents[in.index] = 0.0f;
			array.energies[in.index] = energy;
			return in.id;
		}
		return INVALID_SUID;
	}

	void remove(SpriteArray& array, SUID sid) {
		SpriteArrayIndex& in = array.indices[sid];
		assert(in.index != UINT16_MAX);// , "Invalid index for %d", id);
		//XASSERT(id < capacity, "ID %d out of range %d", id, capacity);
		if (array.num > 1) {
			int last = array.num - 1;
			SUID lastID = array.ids[last];
			SpriteArrayIndex& lastIn = array.indices[lastID];
			if (last != in.index) {
				array.ids[in.index] = array.ids[last];
				array.positions[in.index] = array.positions[last];
				array.scales[in.index] = array.scales[last];
				array.rotations[in.index] = array.rotations[last];
				array.textures[in.index] = array.textures[last];
				array.colors[in.index] = array.colors[last];				
				array.types[in.index] = array.types[last];
				array.energies[in.index] = array.energies[last];
				array.velocities[in.index] = array.velocities[last];
				array.forces[in.index] = array.forces[last];
				array.previous[in.index] = array.previous[last];
				array.extents[in.index] = array.extents[last];
				
			}
			lastIn.index = in.index;
		}
		in.index = UINT16_MAX;
		array.freeList[array.num_free] = sid;
		++array.num_free;
		//ds::log(LogLevel::LL_DEBUG, "3> adding %d to free list - num: %d", sid, array.num_free);
		--array.num;
	}

	void render(SpriteArray& array, SpriteBatchBuffer* buffer) {
		ds::vec2* positions = array.positions;
		ds::vec2* scales = array.scales;
		float* rotations = array.rotations;
		ds::vec4* textures = array.textures;
		ds::Color* colors = array.colors;
		for (uint32_t i = 0; i < array.num; ++i) {
			buffer->add(*positions, *textures, *scales, *rotations, *colors);
			++positions;
			++rotations;
			++scales;
			++textures;
			++colors;
		}
	}
}