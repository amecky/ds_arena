#pragma once
#include "utils\Stack.h"
#include "states\StateMachine.h"
#include "utils\HexGrid.h"
#include "ElasticBorder.h"
#include "sprites.h"
#include "DataArray.h"
#include "math.h"
#include "FloatArray.h"
#include "particles\Particlesystem.h"
#include "ElasticBorder.h"
#include "utils\hud.h"
#include <stack>
#include "utils\HexGrid.h"

enum EventTypes {
	ET_NONE,
	ET_PREPARE_ELAPSED,
	ET_MAIN_MENU_PLAY,
	ET_MAIN_MENU_EXIT
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

// ---------------------------------------------------------------
// Spawn item
// ---------------------------------------------------------------
struct SpawnItem {
	int type;
	ds::vec2 pos;
};

typedef void(*spawnFunction)(SpawnItem*, int, int);

// ---------------------------------------------------------------
// Player
// ---------------------------------------------------------------
struct Player {
	ds::vec2 pos;
	ds::vec2 previous;
	float angle;
	int energy;
};

// ---------------------------------------------------------------
// Bullet
// ---------------------------------------------------------------
struct Bullet {
	ID id;
	ds::vec2 pos;
	ds::vec2 velocity;
	float angle;
};

enum EnemyState {
	ES_STARTING,
	ES_MOVING
};

// ---------------------------------------------------------------
// Enemy
// ---------------------------------------------------------------
struct Enemy {
	ID id;
	ds::vec2 pos;
	ds::vec2 velocity;
	ds::vec2 force;
	float angle;
	float timer;
	EnemyState state;
	ds::vec2 scale;
	int energy;
	int type;
};

// ---------------------------------------------------------------
// game settings
// ---------------------------------------------------------------
struct GameSettings {
	int maxSpawnEnemies;
	ds::Color playerHightlightColor;
	ds::Color wakeUpHightlightColor;
};

// ---------------------------------------------------------------
// MainState
// ---------------------------------------------------------------
class MainState : public GameState {

public:
	MainState(BackgroundState* backgroundState);
	virtual ~MainState();
	int tick(float dt, EventStream* stream);
	void render();
	void startSpawning();
	void stopSpawning();
	void activate();
	void deactivate() {
		_active = false;
	}
private:
	void movePlayer(float dt);
	void moveBullets(float dt);
	void moveEnemies(float dt);
	void spawnEnemies(float dt);
	void spawn(float dt);
	void handleCollisions();
	void handleShooting();
	bool handlePlayerCollision();
	void startGame();
	Player _player;
	DataArray<Bullet> _bullets;
	DataArray<Enemy> _enemies;
	bool _shooting;
	float _shootingTimer;
	float _spawnTimer;
	float _spawnQueueTimer;
	AbstractPath<float> _scalePath;
	ParticleManager* _particleManager;
	PSUID _enemyExplosion;
	PSUID _playerTrail;
	PSUID _wakeUpSystem;
	ParticlesystemInstanceSettings _explosionSettings;
	ParticlesystemInstanceSettings _bulletExplosionSettings;
	ParticlesystemInstanceSettings _playerTrailSettings;
	ParticlesystemInstanceSettings _wakeupSettings;
	std::stack<SpawnItem> _spawnItems;
	GameSettings _gameSettings;
	BackgroundState* _backgroundState;
	HUD _hud;
	spawnFunction _spawnFunctions[8];
	bool _spawning;
};
