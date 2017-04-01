#pragma once
#include "sprites.h"
#include "DataArray.h"
#include "math.h"
#include "FloatArray.h"
#include "particles\Particlesystem.h"
// ---------------------------------------------------------------
// Player
// ---------------------------------------------------------------
struct Player {
	ds::vec2 pos;
	float angle;
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

struct ExplosionSettings {
	int count;
	ds::vec2 ttl;
	float angleVariance;
	float radiusVariance;
	ds::vec2 velocityVariance;
	ds::vec2 sizeVariance;
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
	void handleCollisions();
	void handleShooting();
	void emittExplosion(Particlesystem* system, const ExplosionSettings& settings, float px, float py, float radius);
	Player _player;
	DataArray<Bullet> _bullets;
	DataArray<Enemy> _enemies;
	bool _shooting;
	float _shootingTimer;
	float _spawnTimer;
	AbstractPath<float> _scalePath;
	ParticleManager* _particleManager;
	Particlesystem* _enemyExplosion;
	ExplosionSettings _explosionSettings;
};

