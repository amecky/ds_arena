#include "Game.h"
#include <cmath>
#include "..\..\diesel\diesel.h"
#include "utils\json.h"

const static float SHOOT_TTL = 0.2f;
const static float SPAWN_QUEUE_TTL = 0.2f;
const static float SPAWN_DELAY = 5.0f;

const static ds::vec4 ENEMY_TEXTURES[] = {
	ds::vec4(120,0,32,32),
	ds::vec4(160,0,32,32),
};

// ---------------------------------------------------------------
// circle spawn function
// ---------------------------------------------------------------
void spawnCircle(SpawnItem* items, int count, int type) {
	ds::vec2 p = ds::vec2(512, 354);
	int start = ds::random(0, 18.0f);
	float step = ds::TWO_PI / 18.0f;
	float angle = start * step;
	int cnt = 0;
	for (int i = 0; i < count; ++i) {
		SpawnItem item;
		item.pos = ds::vec2(cos(angle), sin(angle)) * 270.0f + p;
		item.type = type;
		items[cnt++] = item;
		angle += step;
	}
}

// ---------------------------------------------------------------
// left line spawn function
// ---------------------------------------------------------------
void spawnLeftLine(SpawnItem* items, int count, int type) {
	int cnt = 0;
	for (int i = 0; i < count; ++i) {
		SpawnItem item;
		item.pos = ds::vec2(100, 120 + i * 50);
		item.type = type;
		items[cnt++] = item;
	}
}

// ---------------------------------------------------------------
// right line spawn function
// ---------------------------------------------------------------
void spawnRightLine(SpawnItem* items, int count, int type) {
	int cnt = 0;
	for (int i = 0; i < count; ++i) {
		SpawnItem item;
		item.pos = ds::vec2(900, 120 + i * 50);
		item.type = type;
		items[cnt++] = item;
	}
}

// ---------------------------------------------------------------
// top line spawn function
// ---------------------------------------------------------------
void spawnTopLine(SpawnItem* items, int count, int type) {
	int cnt = 0;
	for (int i = 0; i < count; ++i) {
		SpawnItem item;
		item.pos = ds::vec2(120 + i * 50, 650);
		item.type = type;
		items[cnt++] = item;
	}
}

// ---------------------------------------------------------------
// bottom line spawn function
// ---------------------------------------------------------------
void spawnBottomLine(SpawnItem* items, int count, int type) {
	int cnt = 0;
	for (int i = 0; i < count; ++i) {
		SpawnItem item;
		item.pos = ds::vec2(120 + i * 50, 80);
		item.type = type;
		items[cnt++] = item;
	}
}



void loadSettings(const SJSONReader& reader, const char* catName, ExplosionSettings* settings) {
	reader.get("count", &settings->count,catName);
	reader.get("angle_variance", &settings->angleVariance, catName);
	reader.get("radius_variance", &settings->radiusVariance, catName);
	reader.get("ttl", &settings->ttl, catName);
	reader.get("velocity_variance", &settings->velocityVariance, catName);
	reader.get("size_variance", &settings->sizeVariance, catName);
}

Game::Game() {
	_player.pos = ds::vec2(512, 384);
	_player.previous = _player.pos;
	_player.angle = 0.0f;
	_player.energy = 100;
	_shooting = false;
	_shootingTimer = 0.0f;
	_spawnTimer = SPAWN_DELAY - SPAWN_DELAY * 0.1f;
	_spawnQueueTimer = 0.0f;
	_scalePath.add(0.0f, 0.2f);
	_scalePath.add(0.5f, 1.5f);
	_scalePath.add(0.75f, 0.6f);
	_scalePath.add(1.0f, 1.0f);

	_spawnFunctions[0] = spawnCircle;
	_spawnFunctions[1] = spawnLeftLine;
	_spawnFunctions[2] = spawnRightLine;
	_spawnFunctions[3] = spawnTopLine;
	_spawnFunctions[4] = spawnBottomLine;

	RID textureID = ds::findResource(SID("content\\TextureArray.png"), ds::ResourceType::RT_SRV);
	_particleManager = new ParticleManager(4096, textureID);

	_enemyExplosion = _particleManager->load("explosion", textureID);
	_playerTrail = _particleManager->load("player_trail", textureID);
	_wakeUpSystem = _particleManager->load("wake_up", textureID);

	SJSONReader reader;
	reader.parse("content\\particlesystems.json");

	loadSettings(reader, "explosion_settings", &_explosionSettings);
	loadSettings(reader, "bullet_explosion_settings", &_bulletExplosionSettings);
	loadSettings(reader, "player_trail_settings", &_playerTrailSettings);
	loadSettings(reader, "wake_up_settings", &_wakeupSettings);

	SJSONReader settingsReader;
	settingsReader.parse("content\\settings.json");
	settingsReader.get("max_spawn_enemies", &_gameSettings.maxSpawnEnemies, "settings");

	
	settingsReader.get("Tension", &_borderSettings.Tension, "border_settings");
	settingsReader.get("Dampening", &_borderSettings.Dampening, "border_settings");
	settingsReader.get("Spread", &_borderSettings.Spread, "border_settings");
	settingsReader.get("numX", &_borderSettings.numX, "border_settings");
	settingsReader.get("numY", &_borderSettings.numY, "border_settings");
	settingsReader.get("thickness", &_borderSettings.thickness, "border_settings");
	settingsReader.get("verticalTexture", &_borderSettings.verticalTexture, "border_settings");
	settingsReader.get("horizontalTexture", &_borderSettings.horizontalTexture, "border_settings");
	settingsReader.get("targetHeight", &_borderSettings.targetHeight, "border_settings");
	settingsReader.get("splashForce", &_borderSettings.splashForce, "border_settings");
	settingsReader.get("length", &_borderSettings.length, "border_settings");

	_borderSettings.textureID = textureID;
	_borders = new ElasticBorder(&_borderSettings);
	
	_hud.reset();
}


Game::~Game() {
	delete _enemyExplosion;
	delete _particleManager;
}

void Game::emittTrail(Particlesystem* system, const ExplosionSettings& settings, float px, float py, float radius) {
	for (int i = 0; i < settings.count; ++i) {
		float angle = ds::TWO_PI * static_cast<float>(i) / static_cast<float>(settings.count);
		float x = px + cos(angle) * (radius + ds::random(-settings.radiusVariance, settings.radiusVariance));
		float y = py + sin(angle) * (radius + ds::random(-settings.radiusVariance, settings.radiusVariance));
		float da = angle * settings.angleVariance;
		angle += ds::random(-da, da);
		ds::vec2 s = ds::vec2(0.2f, 0.15f);
		float ds = ds::random(settings.sizeVariance.x, settings.sizeVariance.y);
		s *= ds;
		float ttl = ds::random(settings.ttl.x, settings.ttl.y);
		float rotation = angle;
		system->add(ds::vec2(x, y), ds::vec2(0.0f), ds::vec2(0.0f), ttl, rotation);
	}
}


void Game::emittExplosion(Particlesystem* system, const ExplosionSettings& settings, float px, float py, float radius) {
	for (int i = 0; i < settings.count; ++i) {
		float angle = ds::TWO_PI * static_cast<float>(i) / static_cast<float>(settings.count);
		float x = px + cos(angle) * (radius + ds::random(-settings.radiusVariance, settings.radiusVariance));
		float y = py + sin(angle) * (radius + ds::random(-settings.radiusVariance, settings.radiusVariance));
		float da = angle * settings.angleVariance;
		angle += ds::random(-da, da);
		ds::vec2 s = ds::vec2(0.2f, 0.15f);
		float ds = ds::random(settings.sizeVariance.x, settings.sizeVariance.y);
		s *= ds;
		float ttl = ds::random(settings.ttl.x, settings.ttl.y);
		float rotation = angle;
		ds::vec2 velocity = ds::random(settings.velocityVariance.x, settings.velocityVariance.y) * ds::vec2(cos(angle), sin(angle));
		ds::vec2 acc = velocity * -0.5f;
		system->add(ds::vec2(x, y), velocity, acc, ttl, rotation);
	}
}
// ---------------------------------------------------------------
// tick
// ---------------------------------------------------------------
void Game::tick(float dt) {

	_borders->tick(dt);

	movePlayer(dt);

	handleShooting();

	moveBullets(dt);

	moveEnemies(dt);

	handleCollisions();

	handlePlayerCollision();

	spawn(dt);

	spawnEnemies(dt);

	_particleManager->tick(dt);

}

// ---------------------------------------------------------------
// spawn enemies
// ---------------------------------------------------------------
void Game::spawn(float dt) {
	_spawnTimer += dt;
	if (_spawnTimer >= SPAWN_DELAY) {
		_spawnTimer -= SPAWN_DELAY;
		SpawnItem items[32];
		int f = ds::random(0.0f, 4.9f);
		int t = ds::random(0.0f, 1.9f);
		_spawnFunctions[f](items, 10, t);
		for (int i = 0; i < 10; ++i) {
			_spawnItems.push(items[i]);
		}
		/*
		ds::vec2 p = ds::vec2(512, 354);// +ds::vec2(ds::random(-100.0f, 100.0f), ds::random(-100.0f, 100.0f));
		int start = ds::random(0, _gameSettings.maxSpawnEnemies);
		float step = ds::TWO_PI / static_cast<float>(_gameSettings.maxSpawnEnemies);
		float angle = start * step;
		for (int i = 0; i < 10; ++i) {
			SpawnItem item;
			item.pos = ds::vec2(cos(angle), sin(angle)) * 270.0f + p;
			item.type = 1;
			_spawnItems.push(item);
			angle += step;
		}
		*/
	}
}

// ---------------------------------------------------------------
// spawn from spawn queue
// ---------------------------------------------------------------
void Game::spawnEnemies(float dt) {
	if ( !_spawnItems.empty()) {
		_spawnQueueTimer += dt;
		if (_spawnQueueTimer >= SPAWN_QUEUE_TTL) {
			_spawnQueueTimer -= SPAWN_QUEUE_TTL;
			ds::vec2 p = _spawnItems.top().pos;
			int type = _spawnItems.top().type;
			_spawnItems.pop();
			ID id = _enemies.add();
			Enemy& e = _enemies.get(id);
			e.pos = p;
			e.id = id;
			e.angle = 0.0f;
			e.force = ds::vec2(0.0f);
			e.velocity = ds::vec2(0.0f);
			e.state = ES_STARTING;
			e.timer = 0.0f;
			e.scale = ds::vec2(1.0f);
			e.energy = 3;
			e.type = type;
			emittTrail(_wakeUpSystem, _wakeupSettings, p.x, p.y, 5.0f);
		}
	}
}

// ---------------------------------------------------------------
// handle shooting
// ---------------------------------------------------------------
void Game::handleShooting() {
	if (ds::isMouseButtonPressed(0)) {
		_shooting = true;
	}
	else {
		_shooting = false;
	}

	if (_shooting) {
		_shootingTimer += static_cast<float>(ds::getElapsedSeconds());
		if (_shootingTimer >= SHOOT_TTL) {
			_shootingTimer -= SHOOT_TTL;
			ID id = _bullets.add();
			Bullet& b = _bullets.get(id);
			b.pos = _player.pos;
			b.angle = _player.angle;
			b.velocity = ds::vec2(cos(b.angle), sin(b.angle)) * 600.0f;
		}
	}
}

// ---------------------------------------------------------------
// handle player collisions
// ---------------------------------------------------------------
bool Game::handlePlayerCollision() {
	float dist = 0.0f;
	ds::vec2 pnv;
	bool hit = false;
	DataArray<Enemy>::iterator eit = _enemies.begin();
	while (eit != _enemies.end()) {
		if (eit->state == ES_MOVING) {
			if (math::checkCircleIntersection(_player.pos, 20.0f, eit->pos, 20.0f, &dist, &pnv)) {
				emittExplosion(_enemyExplosion, _explosionSettings, eit->pos.x, eit->pos.y, 10.0f);
				eit = _enemies.remove(eit->id);
				_player.energy -= 10;
				_hud.decreaseHealth(10);
				hit = true;
			}
			else {
				++eit;
			}
		}
		else {
			++eit;
		}
	}
	return hit;
}

// ---------------------------------------------------------------
// handle collisions
// ---------------------------------------------------------------
void Game::handleCollisions() {
	float dist = 0.0f;
	ds::vec2 pnv;
	DataArray<Bullet>::iterator it = _bullets.begin();
	bool hit = false;
	while (it != _bullets.end()) {
		hit = false;
		DataArray<Enemy>::iterator eit = _enemies.begin();
		while (eit != _enemies.end()) {
			if (eit->state == ES_MOVING) {
				if (math::checkCircleIntersection(it->pos, 5.0f, eit->pos, 20.0f, &dist, &pnv)) {
					if (_bullets.contains(it->id)) {
						emittExplosion(_enemyExplosion, _bulletExplosionSettings, it->pos.x, it->pos.y, 5.0f);						
						it = _bullets.remove(it->id);
						hit = true;						
					}
					--eit->energy;
					if (eit->energy <= 0) {
						emittExplosion(_enemyExplosion, _explosionSettings, eit->pos.x, eit->pos.y, 10.0f);						
						eit = _enemies.remove(eit->id);
						_hud.addScore(50);
					}
					else {
						++eit;
					}
				}
				else {
					++eit;
				}
			}
			else {
				++eit;
			}
		}
		if (!hit) {
			++it;
		}
	}
}
// ---------------------------------------------------------------
// move bullets
// ---------------------------------------------------------------
void Game::moveBullets(float dt) {
	DataArray<Bullet>::iterator it = _bullets.begin();
	while (it != _bullets.end()) {
		it->pos += it->velocity * static_cast<float>(ds::getElapsedSeconds());
		if (it->pos.x < 0.0f || it->pos.x > 1024.0f || it->pos.y < 0.0f || it->pos.y > 768.0f) {
			emittExplosion(_enemyExplosion, _bulletExplosionSettings, it->pos.x, it->pos.y, 5.0f);
			it = _bullets.remove(it->id);
		}
		else if (_borders->collides(it->pos, 4.0f)) {
			emittExplosion(_enemyExplosion, _bulletExplosionSettings, it->pos.x, it->pos.y, 5.0f);
			it = _bullets.remove(it->id);
		}
		else {
			++it;
		}
	}
}

// ---------------------------------------------------------------
// move enemies
// ---------------------------------------------------------------
void Game::moveEnemies(float dt) {
	//
	// seek player
	//
	DataArray<Enemy>::iterator eit = _enemies.begin();
	ds::vec2 target = _player.pos;
	while (eit != _enemies.end()) {		
		if (eit->state == ES_MOVING) {
			ds::vec2 diff = target - eit->pos;
			ds::vec2 n = normalize(diff);
			eit->force = n * 100.0f;
		}
		else if (eit->state == ES_STARTING) {
			eit->timer += dt;
			float norm = eit->timer / 0.5f;
			if (norm >= 1.0f) {
				eit->state = ES_MOVING;
				eit->scale = ds::vec2(1.0f);
			}
			else {
				float s = 0.0f;
				_scalePath.get(norm, &s);
				eit->scale = ds::vec2(s);
			}
		}
		++eit;
	}

	const float minDistance = 30.0f;
	const float relaxation = 10.0f;
	//
	// separate enemies
	//
	eit = _enemies.begin();
	while (eit != _enemies.end()) {
		if ( eit->state == ES_MOVING) {
			DataArray<Enemy>::iterator nit = _enemies.begin();
			while (nit != _enemies.end()) {
				ds::vec2 currentPos = eit->pos;
				if (nit->id != eit->id) {
					float sqrDist = minDistance * minDistance;
					ds::vec2 dist = nit->pos - currentPos;
					if (sqr_length(dist) < sqrDist) {
						ds::vec2 separationForce = dist;
						separationForce = normalize(separationForce);
						separationForce = separationForce * relaxation;
						eit->force -= separationForce;
					}
				}
				++nit;
			}
		}
		++eit;
	}
	//
	// apply force
	//
	eit = _enemies.begin();
	while (eit != _enemies.end()) {
		eit->pos += eit->force * dt;
		eit->angle = math::getAngle(eit->pos, target);
		eit->force = ds::vec2(0.0f);
		++eit;
	}
}

// ---------------------------------------------------------------
// move player
// ---------------------------------------------------------------
void Game::movePlayer(float dt) {
	ds::vec2 vel = ds::vec2(0.0f);
	if (ds::isKeyPressed('A')) {
		vel.x -= 1.0f;
	}
	if (ds::isKeyPressed('D')) {
		vel.x += 1.0f;
	}
	if (ds::isKeyPressed('W')) {
		vel.y += 1.0f;
	}
	if (ds::isKeyPressed('S')) {
		vel.y -= 1.0f;
	}
	ds::vec2 pos = _player.pos + vel * 250.0f * dt;
	if (pos.x > 60.0f && pos.x < 960.0f && pos.y > 80.0f && pos.y < 650.0f) {
		_player.pos = pos;

		ds::vec2 d = _player.pos - _player.previous;
		if (sqr_length(d) > 100.0f) {
			_player.previous = _player.pos;
			emittTrail(_playerTrail, _playerTrailSettings, _player.previous.x, _player.previous.y, 10.0f);
		}
	}
	ds::vec2 mp = ds::getMousePosition();
	_player.angle = math::getAngle(_player.pos, mp);
}

// ---------------------------------------------------------------
// render
// ---------------------------------------------------------------
void Game::render() {
	sprites::begin();
	sprites::add(ds::vec2(512, 384), ds::vec4(512, 0, 512, 384), ds::vec2(2, 2));
	sprites::flush();
	_borders->render();
	_particleManager->render();
	sprites::begin();
	_hud.render();
	DataArray<Bullet>::iterator it = _bullets.begin();
	while (it != _bullets.end()) {
		sprites::add(it->pos, ds::vec4(120, 60, 8, 8), ds::vec2(3.0f, 0.5f), it->angle, ds::Color(42,202,236,255));
		++it;
	}
	DataArray<Enemy>::iterator eit = _enemies.begin();
	while (eit != _enemies.end()) {
		sprites::add(eit->pos, ENEMY_TEXTURES[eit->type], eit->scale, eit->angle);
		++eit;
	}
	sprites::add(_player.pos, ds::vec4(0, 40, 40, 40), ds::vec2(1, 1), _player.angle);
	sprites::flush();

	ds::dbgPrint(0, 3, "Spawn timer: %g", _spawnTimer);
}
