#include "hud.h"
#include "..\..\..\diesel\diesel.h"
#include "sprites.h"

const ds::vec4 NUMBERS[] = {
	ds::vec4(0,120,40,40),
	ds::vec4(40,120,32,40),
	ds::vec4(72,120,38,40),
	ds::vec4(111,120,40,40),
	ds::vec4(151,120,38,40),
	ds::vec4(189,120,40,40),
	ds::vec4(229,120,38,40),
	ds::vec4(267,120,40,40),
	ds::vec4(307,120,38,40),
	ds::vec4(345,120,40,40)
};

namespace numbers {

	void draw(const ds::vec2& pos, int value, int num, bool leadingZeros) {
		ds::vec2 hp = pos;
		int idx = 0;
		int tmp = value;
		int div = 1;
		for (int i = 0; i < num; ++i) {
			if (i > 0) {
				div *= 10;
			}
		}
		for (int i = 0; i < num; ++i) {
			int r = tmp / div;
			tmp = tmp - r * div;
			div /= 10;
			const ds::vec4& t = NUMBERS[r];
			if (r >= 0 && r < 10) {
				sprites::add(hp, t);
				hp.x += t.z + 2.0f;
			}
		}
	}

}

void HUD::reset() {
	_health = 100;
	_score = 0;
	updateScore();
	updateHealth();
}

void HUD::render() {
	ds::vec2 hp = ds::vec2(80, 720);
	for (int i = 0; i < 3; ++i) {
		if (_healthNumbers[i] != -1) {
			const ds::vec4& t = NUMBERS[_healthNumbers[i]];
			sprites::add(hp, t);
			hp.x += t.z + 5.0f;
		}
	}
	hp = ds::vec2(700, 720);
	for (int i = 0; i < 6; ++i) {
		if (_scoreNumbers[i] != -1) {
			const ds::vec4& t = NUMBERS[_scoreNumbers[i]];
			sprites::add(hp, t);
			hp.x += t.z + 5.0f;
		}
	}
}

void HUD::addScore(int score) {
	_score += score;
	updateScore();
}

void HUD::updateScore() {
	int tmp = _score;
	int f = 100000;
	for (int i = 0; i < 6; ++i) {
		int r = tmp / f;
		_scoreNumbers[i] = r;
		tmp = tmp - r * f;
		f /= 10;
	}
}

void HUD::updateHealth() {
	int tmp = _health;
	if (_health < 100) {
		_healthNumbers[0] = -1;
		int f = _health / 10;
		_healthNumbers[1] = f;
		int s = (_health - f * 10);
		_healthNumbers[2] = s;
	}
	else {
		_healthNumbers[0] = 1;
		_healthNumbers[1] = 0;
		_healthNumbers[2] = 0;
	}

}

void HUD::decreaseHealth(int h) {
	_health -= h;
	if (_health < 0) {
		_health = 0;
	}
	updateHealth();
}

