#include "hud.h"

const ds::vec4 NUMBERS[] = {
	{ 440,0,27,22 },
	{ 467,0,17,22 },
	{ 484,0,26,22 },
	{ 510,0,26,22 },
	{ 536,0,27,22 },
	{ 563,0,27,22 },
	{ 590,0,26,22 },
	{ 616,0,27,22 },
	{ 643,0,25,22 },
	{ 668,0,26,22 },

};

namespace numbers {

	void draw(SpriteBatchBuffer* buffer,const ds::vec2& pos, int value, int num, bool leadingZeros, const ds::Color& color) {
		ds::vec2 hp = pos;
		int idx = 0;
		int tmp = value;
		int div = 1;
		for (int i = 0; i < num; ++i) {
			if (i > 0) {
				div *= 10;
			}
		}
		bool printed = false;
		for (int i = 0; i < num; ++i) {
			int r = tmp / div;
			tmp = tmp - r * div;
			div /= 10;
			if (leadingZeros) {
				const ds::vec4& t = NUMBERS[r];
				if (r >= 0 && r < 10) {
					buffer->add(hp, t, ds::vec2_ONE,0.0f,color);
					hp.x += t.z + 2.0f;
				}
			}
			else {
				if (printed) {
					const ds::vec4& t = NUMBERS[r];
					if (r >= 0 && r < 10) {
						buffer->add(hp, t, ds::vec2_ONE, 0.0f, color);
						hp.x += t.z + 2.0f;
					}
				}
				else {
					if (r > 0) {
						const ds::vec4& t = NUMBERS[r];
						if (r >= 0 && r < 10) {
							buffer->add(hp, t, ds::vec2_ONE, 0.0f, color);
							hp.x += t.z + 2.0f;
						}
						printed = true;
					}
				}
			}
		}
	}

}
