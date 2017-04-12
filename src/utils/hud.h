#pragma once
#include <stdint.h>
#include "..\..\..\diesel\diesel.h"

namespace numbers {

	void draw(const ds::vec2& pos, int value, int num, bool leadingZeros = true);

}
class HUD {

public:
	void reset();
	void render();
	void addScore(int s);
	void decreaseHealth(int h);
private:
	void updateScore();
	void updateHealth();
	int _health;
	int _healthNumbers[3];
	int _score;
	int _scoreNumbers[6];
};

