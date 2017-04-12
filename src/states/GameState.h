#pragma once
#include "StateMachine.h"
#include "..\lib\HexGrid.h"
#include "..\utils\ElasticBorder.h"
#include "..\utils\sprites.h"
//#include "..\lib\DataArray.h"
//#include "..\lib\math.h"
//#include "..\lib\FloatArray.h"
//#include "..\particles\Particlesystem.h"
//#include "..\utils\hud.h"
//#include <stack>
//#include "..\lib\HexGrid.h"

enum EventTypes {
	ET_NONE,
	ET_PREPARE_ELAPSED,
	ET_MAIN_MENU_PLAY,
	ET_MAIN_MENU_EXIT,
	ET_GAME_OVER_PLAY,
	ET_GAME_OVER_EXIT,
	ET_PLAYER_KILLED
};

// ---------------------------------------------------------------
// PrepareState
// ---------------------------------------------------------------
class PrepareState : public GameState {

public:
	PrepareState() : GameState("PrepareState") {}
	int tick(float dt, EventStream* stream);
	void render();
	void activate();
	void deactivate();
private:
	float _prepareTimer;
};

// ---------------------------------------------------------------
// GameOverState
// ---------------------------------------------------------------
class GameOverState : public GameState {

public:
	GameOverState() : GameState("GameOverState") {}
	int tick(float dt, EventStream* stream);
	void render();
	void activate() {
		_active = true;
	}
	void deactivate() {
		_active = false;
	}
};

// ---------------------------------------------------------------
// MainMenuState
// ---------------------------------------------------------------
class MainMenuState : public GameState {

public:
	MainMenuState() : GameState("MainMenuState") {}
	int tick(float dt, EventStream* stream);
	void render();
	void activate() {
		_active = true;
	}
	void deactivate() {
		_active = false;
	}
};
// ---------------------------------------------------------------
// BackgroundState
// ---------------------------------------------------------------
class HexGrid;

class BackgroundState : public GameState {

public:
	BackgroundState();
	~BackgroundState();
	int tick(float dt, EventStream* stream);
	void render();
	void activate() {
		_active = true;
	}
	void deactivate() {
		_active = false;
	}
	Hex convert(const ds::vec2& p) {
		return _grid.convert(p);
	}
	ds::vec2 convert(const Hex& h) {
		return _grid.convert(h);
	}
	void highlight(const ds::vec2& p, const ds::Color& color);
	bool borderCollision(const ds::vec2& p, float radius);
private:
	HexGrid _grid;
	int _height;
	int _width;
	ElasticBorderSettings _borderSettings;
	ElasticBorder* _borders;
};

