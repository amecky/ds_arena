#pragma once
#include "sprites.h"
#include "DataArray.h"
#include "math.h"
#include "FloatArray.h"
#include "particles\Particlesystem.h"
#include "ElasticBorder.h"
#include "utils\hud.h"
#include <stack>
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
};

struct SpawnItem {
	int type;
	ds::vec2 pos;
};

struct ExplosionSettings {
	int count;
	ds::vec2 ttl;
	float angleVariance;
	float radiusVariance;
	ds::vec2 velocityVariance;
	ds::vec2 sizeVariance;
};

struct GameSettings {
	int maxSpawnEnemies;
};

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
	void emittExplosion(Particlesystem* system, const ExplosionSettings& settings, float px, float py, float radius);
	void emittTrail(Particlesystem* system, const ExplosionSettings& settings, float px, float py, float radius);
	Player _player;
	DataArray<Bullet> _bullets;
	DataArray<Enemy> _enemies;
	bool _shooting;
	float _shootingTimer;
	float _spawnTimer;
	float _spawnQueueTimer;
	AbstractPath<float> _scalePath;
	ParticleManager* _particleManager;
	Particlesystem* _enemyExplosion;
	Particlesystem* _playerTrail;
	ExplosionSettings _explosionSettings;
	ExplosionSettings _bulletExplosionSettings;
	ExplosionSettings _playerTrailSettings;
	std::stack<SpawnItem> _spawnItems;
	GameSettings _gameSettings;
	ElasticBorderSettings _borderSettings;
	ElasticBorder* _borders;
	HUD _hud;
};

