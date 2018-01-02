#include "math.h"
#include <cmath>

namespace math {

	// ---------------------------------------------------------------
	// get angle between two ds::vec2 vectors
	// ---------------------------------------------------------------
	float getAngle(const ds::vec2& u, const ds::vec2& v) {
		double x = v.x - u.x;
		double y = v.y - u.y;
		double ang = atan2(y, x);
		return (float)ang;
	}

	float clamp(float value, float min, float max) {
		if (value < min) {
			return min;
		}
		if (value > max) {
			return max;
		}
		return value;
	}

	// -------------------------------------------------------
	// Check if two circles overlap
	// -------------------------------------------------------
	bool checkCircleIntersection(const ds::vec2& p1, float r1, const ds::vec2& p2, float r2, float* dist, ds::vec2* penVec) {
		ds::vec2 diff = p1 - p2;
		float distance = sqr_length(diff);
		float rsqr = (r1 + r2) * (r1 + r2);
		float d = length(diff) - (r1 + r2);
		if (dist != 0) {
			*dist = d;
		}
		if (penVec != 0) {
			*penVec = normalize(diff);
			*penVec = *penVec * (*dist);
		}
		if (distance <= rsqr) {
			return true;
		}
		return false;
	}

	int out_of_bounds(const ds::vec2& pos, const ds::vec4& box) {
		int ret = 0;
		if (pos.x < box.x) {
			ret += 1;
		}
		if (pos.x > box.z) {
			ret += 4;
		}
		if (pos.y > box.w) {
			ret += 2;
		}
		if (pos.y < box.y) {
			ret += 8;
		}
		return ret;
	}

}