#pragma once
#include "sprites.h"
#include "DataArray.h"
#include "math.h"
#include "FloatArray.h"
#include "particles\Particlesystem.h"
#include "ElasticBorder.h"
#include "utils\hud.h"
#include <stack>
#include "utils\HexGrid.h"
#include "GameState.h"

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

enum GameLayers {
	GL_NONE,
	GL_BACKGROUND,
	GL_PREPARE,
	GL_PLAYER,
	GL_ENEMIES,
	GL_BULLETS,
	GL_HUD,
	GL_GAME_OVER,
	GL_START
};

enum GameMode {
	GM_START,
	GM_PREPARE,
	GM_MAIN,
	GM_GAME_OVER
};

struct GameLayerDefinition {
	GameMode mode;
	int renderMask;
	int updateMask;
};

const static GameLayerDefinition GAME_MODES[] = {
	{ GM_START, 1 << GL_BACKGROUND | 1 << GL_PREPARE | 1 << GL_PLAYER | 1 << GL_BULLETS , 1 << GL_BACKGROUND | 1 << GL_PREPARE | 1 << GL_PLAYER | 1 << GL_BULLETS },
	{ GM_PREPARE, 1 << GL_BACKGROUND | 1 << GL_PREPARE | 1 << GL_PLAYER | 1 << GL_BULLETS , 1 << GL_BACKGROUND | 1 << GL_PREPARE | 1 << GL_PLAYER | 1 << GL_BULLETS },
	{ GM_MAIN, 1 << GL_BACKGROUND | 1 << GL_PREPARE | 1 << GL_PLAYER | 1 << GL_BULLETS , 1 << GL_BACKGROUND | 1 << GL_PREPARE | 1 << GL_PLAYER | 1 << GL_BULLETS },
	{ GM_GAME_OVER, 1 << GL_BACKGROUND | 1 << GL_PREPARE | 1 << GL_PLAYER | 1 << GL_BULLETS , 1 << GL_BACKGROUND | 1 << GL_PREPARE | 1 << GL_PLAYER | 1 << GL_BULLETS }
};
// ---------------------------------------------------------------
// Game
// ---------------------------------------------------------------
class Game {

public:
	Game();
	~Game();
	void tick(float dt);
	void render();
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
	
	HUD _hud;
	spawnFunction _spawnFunctions[8];
	
	
	uint16_t _updateMask;
	uint16_t _renderMask;

	GameStateManager _gameStates;
	PrepareState* _prepareState;
	BackgroundState* _backgroundState;
	GameOverState* _gameOverState;
};

