#pragma once
#include "ArenaStateMachine.h"
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
class PrepareState : public ArenaGameState {

public:
	PrepareState(GameContext* ctx) : ArenaGameState(ctx, "PrepareState") , _prepareTimer(0.0f) {
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
class GameOverState : public ArenaGameState {

public:
	GameOverState(GameContext* ctx) : ArenaGameState(ctx, "GameOverState") {}
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
class HighscoreState : public ArenaGameState {

public:
	HighscoreState(GameContext* ctx) : ArenaGameState(ctx, "HighscoreState") {}
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
class MainMenuState : public ArenaGameState {

public:
	MainMenuState(GameContext* ctx) : ArenaGameState(ctx, "MainMenuState") {}
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

class BackgroundState : public ArenaGameState {

public:
	BackgroundState(GameContext* ctx, const ElasticBorderSettings& borderSettings);
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
	uint32_t _height;
	uint32_t _width;
	ElasticBorderSettings _borderSettings;
	ElasticBorder* _borders;
};

// ---------------------------------------------------------------
// MainMenuState
// ---------------------------------------------------------------
class ParticlesTestState : public ArenaGameState {

public:
	ParticlesTestState(GameContext* ctx) : ArenaGameState(ctx, "ParticlesTestState"), _selectedSystem(-1), _selectedCategory(-1) {
		for (int i = 0; i < 16; ++i) {
			_flags[i] = false;
		}
		_dialogState = 1;
	}
	int tick(float dt, EventStream* stream);
	void render(SpriteBatchBuffer* buffer);
	void activate() {
		_active = true;
		_timer = 100.0f;
		for (int i = 0; i < 16; ++i) {
			_flags[i] = false;
		}
		_flags[3] = true;
		_dropDownState = 1;
		_selectedSystem = -1;
		_dropDownOffset = 0;
		_emitterState = 1;
		_categoryOffset = 0;
		_categoryState = 1;
		_selectedCategory = -1;
	}
	void deactivate() {
		_active = false;
	}
private:
	int _dialogState;
	int _dropDownState;
	int _selectedSystem;
	int _dropDownOffset;
	int _emitterState;
	int _categoryState;
	int _categoryOffset;
	int _selectedCategory;
	float _timer;
	bool _flags[16];
};