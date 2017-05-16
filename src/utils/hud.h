#pragma once
#include <diesel.h>
#include "SpriteBatchBuffer.h"

namespace numbers {

	void draw(SpriteBatchBuffer* buffer,const ds::vec2& pos, int value, int num, bool leadingZeros = true, const ds::Color& color = ds::Color(255,255,255,255));

}

