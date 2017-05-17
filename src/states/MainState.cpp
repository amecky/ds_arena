#include "MainState.h"
#include "..\utils\json.h"

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
	int step = 540 / count;
	for (int i = 0; i < count; ++i) {
		SpawnItem item;
		item.pos = ds::vec2(100, 120 + i * step);
		item.type = type;
		items[cnt++] = item;
	}
}

// ---------------------------------------------------------------
// right line spawn function
// ---------------------------------------------------------------
void spawnRightLine(SpawnItem* items, int count, int type) {
	int cnt = 0;
	int step = 540 / count;
	for (int i = 0; i < count; ++i) {
		SpawnItem item;
		item.pos = ds::vec2(900, 120 + i * step);
		item.type = type;
		items[cnt++] = item;
	}
}

// ---------------------------------------------------------------
// top line spawn function
// ---------------------------------------------------------------
void spawnTopLine(SpawnItem* items, int count, int type) {
	int cnt = 0;
	int step = 880 / count;
	for (int i = 0; i < count; ++i) {
		SpawnItem item;
		item.pos = ds::vec2(120 + i * step, 620);
		item.type = type;
		items[cnt++] = item;
	}
}

// ---------------------------------------------------------------
// bottom line spawn function
// ---------------------------------------------------------------
void spawnBottomLine(SpawnItem* items, int count, int type) {
	int cnt = 0;
	int step = 880 / count;
	for (int i = 0; i < count; ++i) {
		SpawnItem item;
		item.pos = ds::vec2(120 + i * step, 100);
		item.type = type;
		items[cnt++] = item;
	}
}

// ---------------------------------------------------------------
// MainState
// ---------------------------------------------------------------
MainState::MainState(GameContext* ctx, BackgroundState* backgroundState) : GameState(ctx, "MainState"), _backgroundState(backgroundState) {
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
	

	_ctx->score = 0;

}

MainState::~MainState() {
}

int MainState::tick(float dt, EventStream* stream) {
	if (_running) {
		movePlayer(dt);
		handleShooting();
	}
	moveBullets(dt);
	if (_spawning) {
		moveEnemies(dt);
		handleCollisions();
		handlePlayerCollision(stream);
		spawn(dt);
		spawnEnemies(dt);
	}
	if (!_running) {
		_killTimer += dt;
		if (_killTimer >= 0.4f) {
			_killTimer -= 0.4f;
			if (_enemies.numObjects > 0) {
				const Enemy& e = _enemies.first();
				_ctx->particleManager->emitt(_ctx->enemyExplosion, e.pos, _ctx->explosionSettings);
				_enemies.remove(e.id);
			}
		}
	}
	_ctx->particleManager->tick(dt);
	return 0;
}

void MainState::startKilling() {
	_running = false;
	_killTimer = 0.0f;
}

void MainState::startSpawning() {
	_spawning = true;
	_spawnTimer = SPAWN_DELAY - SPAWN_DELAY * 0.1f;
	_spawnQueueTimer = 0.0f;
}

void MainState::stopSpawning() {
	_spawning = false;
}

void MainState::activate() {
	_ctx->score = 0;
	_player.energy = 10;
	_active = true;
	_running = true;
	_spawning = false;
	_enemies.clear();
	_player.pos = ds::vec2(512, 300);
	_player.previous = ds::vec2(512, 300);
}

void MainState::render(SpriteBatchBuffer* buffer) {
	// particles
	buffer->flush();
	_ctx->particleManager->render();
	if (_running) {
		numbers::draw(buffer, ds::vec2(80, 720),_player.energy,3,false);
		numbers::draw(buffer, ds::vec2(750, 720), _ctx->score, 6, true);		
	}
	DataArray<Bullet>::iterator it = _bullets.begin();
	while (it != _bullets.end()) {
		buffer->add(it->pos, ds::vec4(120, 60, 8, 8), ds::vec2(3.0f, 0.5f), it->angle, ds::Color(42, 202, 236, 255));
		++it;
	}

	DataArray<Enemy>::iterator eit = _enemies.begin();
	while (eit != _enemies.end()) {
		buffer->add(eit->pos, ENEMY_TEXTURES[eit->type], eit->scale, eit->angle);
		++eit;
	}
	if (_running) {
		buffer->add(_player.pos, ds::vec4(0, 40, 40, 40), ds::vec2(1, 1), _player.angle);
	}
}

// ---------------------------------------------------------------
// after the prepare phase start a new game
// ---------------------------------------------------------------
void MainState::startGame() {
	_spawnTimer = SPAWN_DELAY - SPAWN_DELAY * 0.1f;
	_spawnQueueTimer = 0.0f;	
	_ctx->score = 0;
}

// ---------------------------------------------------------------
// spawn enemies
// ---------------------------------------------------------------
void MainState::spawn(float dt) {
	_spawnTimer += dt;
	if (_spawnTimer >= SPAWN_DELAY) {
		_spawnTimer -= SPAWN_DELAY;
		SpawnItem items[32];
		int f = ds::random(0.0f, 4.9f);
		int t = ds::random(0.0f, 1.9f);
		_spawnFunctions[f](items, 6, t);
		for (int i = 0; i < 6; ++i) {
			_spawnItems.push(items[i]);
		}

	}
}

// ---------------------------------------------------------------
// spawn from spawn queue
// ---------------------------------------------------------------
void MainState::spawnEnemies(float dt) {
	if (!_spawnItems.empty()) {
		_spawnQueueTimer += dt;
		if (_spawnQueueTimer >= SPAWN_QUEUE_TTL) {
			_spawnQueueTimer -= SPAWN_QUEUE_TTL;
			ds::vec2 p = _spawnItems.top().pos;
			Hex h = _backgroundState->convert(p);
			ds::vec2 np = _backgroundState->convert(h);
			int type = _spawnItems.top().type;
			_spawnItems.pop();
			ID id = _enemies.add();
			Enemy& e = _enemies.get(id);
			e.pos = np;
			e.id = id;
			e.angle = 0.0f;
			e.force = ds::vec2(0.0f);
			e.velocity = ds::vec2(0.0f);
			e.state = ES_STARTING;
			e.timer = 0.0f;
			e.scale = ds::vec2(1.0f);
			e.energy = 3;
			e.type = type;
			_ctx->particleManager->emitt(_ctx->wakeUpSystem, np, _ctx->wakeupSettings);
			_backgroundState->highlight(np, _ctx->settings.wakeUpHightlightColor);
		}
	}
}

// ---------------------------------------------------------------
// handle shooting
// ---------------------------------------------------------------
void MainState::handleShooting() {
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
bool MainState::handlePlayerCollision(EventStream* stream) {
	float dist = 0.0f;
	ds::vec2 pnv;
	bool hit = false;
	DataArray<Enemy>::iterator eit = _enemies.begin();
	while (eit != _enemies.end()) {
		if (eit->state == ES_MOVING) {
			if (math::checkCircleIntersection(_player.pos, 20.0f, eit->pos, 20.0f, &dist, &pnv)) {
				_ctx->particleManager->emitt(_ctx->enemyExplosion, eit->pos, _ctx->explosionSettings);
				eit = _enemies.remove(eit->id);
				_player.energy -= 10;
				if (_player.energy <= 0) {
					_ctx->particleManager->emitt(_ctx->enemyExplosion, _player.pos, _ctx->deathSettings);
					stream->add(ET_PLAYER_KILLED);
				}
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
void MainState::handleCollisions() {
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
						_ctx->particleManager->emitt(_ctx->enemyExplosion, it->pos,_ctx->bulletExplosionSettings);
						it = _bullets.remove(it->id);
						hit = true;
					}
					--eit->energy;
					if (eit->energy <= 0) {
						_ctx->particleManager->emitt(_ctx->enemyExplosion, eit->pos, _ctx->explosionSettings);
						eit = _enemies.remove(eit->id);
						_ctx->score += 50;
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
void MainState::moveBullets(float dt) {
	DataArray<Bullet>::iterator it = _bullets.begin();
	while (it != _bullets.end()) {
		it->pos += it->velocity * static_cast<float>(ds::getElapsedSeconds());
		if (it->pos.x < 0.0f || it->pos.x > 1024.0f || it->pos.y < 0.0f || it->pos.y > 768.0f) {
			_ctx->particleManager->emitt(_ctx->enemyExplosion, it->pos, _ctx->bulletExplosionSettings);
			it = _bullets.remove(it->id);
		}
		else if (_backgroundState->borderCollision(it->pos, 4.0f)) {
			_ctx->particleManager->emitt(_ctx->enemyExplosion, it->pos, _ctx->bulletExplosionSettings);
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
void MainState::moveEnemies(float dt) {
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
		if (eit->state == ES_MOVING) {
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
void MainState::movePlayer(float dt) {
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
			_ctx->particleManager->emitt(_ctx->playerTrail, _player.previous, _ctx->playerTrailSettings);
		}
	}
	ds::vec2 mp = ds::getMousePosition();
	_player.angle = math::getAngle(_player.pos, mp);
	_backgroundState->highlight(_player.pos, _ctx->settings.playerHightlightColor);
}


