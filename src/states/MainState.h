#pragma once
#include "..\lib\Stack.h"
#include "..\lib\HexGrid.h"
#include "..\lib\DataArray.h"
#include "..\lib\math.h"
#include "..\lib\FloatArray.h"
#include "..\particles\Particlesystem.h"
#include "..\utils\hud.h"
#include <stack>
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
// MainState
// ---------------------------------------------------------------
class MainState : public ArenaGameState {

public:
	MainState(GameContext* ctx, BackgroundState* backgroundState);
	virtual ~MainState();
	int tick(float dt, EventStream* stream);
	void render(SpriteBatchBuffer* buffer);
	void startSpawning();
	void stopSpawning();
	void startKilling();
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
	bool handlePlayerCollision(EventStream* stream);
	void startGame();
	Player _player;
	DataArray<Bullet> _bullets;
	DataArray<Enemy> _enemies;
	bool _shooting;
	float _shootingTimer;
	float _spawnTimer;
	float _spawnQueueTimer;
	AbstractPath<float> _scalePath;
	std::stack<SpawnItem> _spawnItems;
	BackgroundState* _backgroundState;
	spawnFunction _spawnFunctions[8];
	bool _spawning;
	bool _running;
	float _killTimer;

};
