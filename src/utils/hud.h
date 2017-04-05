#pragma once

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

