#pragma once
#include "StateMachine.h"
#include "..\lib\HexGrid.h"
#include "..\utils\ElasticBorder.h"
#include "..\utils\GameContext.h"
#include "..\lib\FloatArray.h"

enum EventTypes {
	ET_NONE,
	ET_PREPARE_ELAPSED,
	ET_MAIN_MENU_PLAY,
	ET_MAIN_MENU_EXIT,
	ET_MAIN_MENU_HIGHSCORES,
	ET_GAME_OVER_PLAY,
	ET_GAME_OVER_EXIT,
	ET_HIGHSCORES_EXIT,
	ET_PLAYER_KILLED
};

// ---------------------------------------------------------------
// PrepareState
// ---------------------------------------------------------------
class PrepareState : public GameState {

public:
	PrepareState(GameContext* ctx) : GameState(ctx, "PrepareState") {
		_scalePath.add(0.0f, 0.2f);
		_scalePath.add(0.2f, 1.2f);
		_scalePath.add(0.4f, 0.8f);
		_scalePath.add(0.6f, 1.1f);
		_scalePath.add(0.8f, 0.9f);
		_scalePath.add(1.0f, 1.0f);
	}
	int tick(float dt, EventStream* stream);
	void render(SpriteBatchBuffer* buffer);
	void activate();
	void deactivate();
private:
	float _prepareTimer;
	AbstractPath<float> _scalePath;
};

// ---------------------------------------------------------------
// GameOverState
// ---------------------------------------------------------------
class GameOverState : public GameState {

public:
	GameOverState(GameContext* ctx) : GameState(ctx, "GameOverState") {}
	int tick(float dt, EventStream* stream);
	void render(SpriteBatchBuffer* buffer);
	void activate();
	void deactivate() {
		_active = false;
	}
private:
	int _rank;
};

// ---------------------------------------------------------------
// HighscoreState
// ---------------------------------------------------------------
class HighscoreState : public GameState {

public:
	HighscoreState(GameContext* ctx) : GameState(ctx, "HighscoreState") {}
	int tick(float dt, EventStream* stream);
	void render(SpriteBatchBuffer* buffer);
	void activate() {
		_index = 0;
		_timer = 0.0f;
		_active = true;
	}
	void deactivate() {
		_active = false;
	}
private:
	float _timer;
	int _index;
};
// ---------------------------------------------------------------
// MainMenuState
// ---------------------------------------------------------------
class MainMenuState : public GameState {

public:
	MainMenuState(GameContext* ctx) : GameState(ctx, "MainMenuState") {}
	int tick(float dt, EventStream* stream);
	void render(SpriteBatchBuffer* buffer);
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
	BackgroundState(GameContext* ctx);
	~BackgroundState();
	int tick(float dt, EventStream* stream);
	void render(SpriteBatchBuffer* buffer);
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

// ---------------------------------------------------------------
// MainMenuState
// ---------------------------------------------------------------
class ParticlesTestState : public GameState {

public:
	ParticlesTestState(GameContext* ctx) : GameState(ctx, "ParticlesTestState") {}
	int tick(float dt, EventStream* stream);
	void render(SpriteBatchBuffer* buffer);
	void activate() {
		_active = true;
		_timer = 100.0f;
		_flags[0] = 0;
		_flags[1] = 1;
		_flags[2] = 0;
		_flags[3] = 0;
		for (int i = 0; i < 4; ++i) {
			_pressed[i] = 0;
		}
	}
	void deactivate() {
		_active = false;
	}
private:
	float _timer;
	int _flags[4];
	int _pressed[4];
};