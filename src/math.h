#pragma once
#include "..\..\diesel\diesel.h"

namespace math {

	float getAngle(const ds::vec2& u, const ds::vec2& v);

	float clamp(float value, float min, float max);

	bool checkCircleIntersection(const ds::vec2& p1, float r1, const ds::vec2& p2, float r2, float* dist = 0, ds::vec2* penVec = 0);
}