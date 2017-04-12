#pragma once
#include "..\..\..\diesel\diesel.h"

namespace bit {

	inline void set(uint16_t* v, int index) {
		*v |= 1 << index;
	}

	inline void clear(uint16_t* v, int index) {
		*v &= ~(1 << index);
	}

	inline bool is_set(uint16_t u, int index) {
		int v = 1 << index;
		return (u & v) == v;
	}

	inline void toggle(uint16_t* v, int index) {
		*v ^= 1 << index;
	}

	inline void fill(uint16_t* v, uint16_t value) {
		for (int index = 0; index < 16; ++index) {
			if (is_set(value, index)) {
				set(v, index);
			}
			else {
				clear(v, index);
			}
		}
	}
}


namespace math {

	float getAngle(const ds::vec2& u, const ds::vec2& v);

	float clamp(float value, float min, float max);

	bool checkCircleIntersection(const ds::vec2& p1, float r1, const ds::vec2& p2, float r2, float* dist = 0, ds::vec2* penVec = 0);
}