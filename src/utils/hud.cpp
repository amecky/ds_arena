#include "hud.h"
#include "..\..\..\diesel\diesel.h"
#include "..\sprites.h"

const ds::vec4 NUMBERS[] = {
	ds::vec4(0,120,41,40),
	ds::vec4(41,120,28,40),
	ds::vec4(69,120,38,40),
	ds::vec4(107,120,40,40),
	ds::vec4(147,120,38,40),
	ds::vec4(185,120,40,40),
	ds::vec4(225,120,38,40),
	ds::vec4(263,120,40,40),
	ds::vec4(303,120,38,40),
	ds::vec4(341,120,40,40)
};

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

