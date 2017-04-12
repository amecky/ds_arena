#pragma once
#include "StateMachine.h"
#include "..\lib\HexGrid.h"
#include "..\utils\ElasticBorder.h"
#include "..\utils\sprites.h"
#include "..\utils\GameContext.h"

enum EventTypes {
	ET_NONE,
	ET_PREPARE_ELAPSED,
	ET_MAIN_MENU_PLAY,
	ET_MAIN_MENU_EXIT,
	ET_MAIN_MENU_HIGHSCORES,
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
	GameOverState(GameContext* ctx) : GameState("GameOverState"), _ctx(ctx) {}
	int tick(float dt, EventStream* stream);
	void render();
	void activate();
	void deactivate() {
		_active = false;
	}
private:
	int _rank;
	GameContext* _ctx;
};

// ---------------------------------------------------------------
// HighscoreState
// ---------------------------------------------------------------
class HighscoreState : public GameState {

public:
	HighscoreState(GameContext* ctx) : GameState("HighscoreState"), _ctx(ctx) {}
	int tick(float dt, EventStream* stream);
	void render();
	void activate() {
		_active = true;
	}
	void deactivate() {
		_active = false;
	}
private:
	GameContext* _ctx;
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

