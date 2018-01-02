#include "MainState.h"
#include <ds_imgui.h>
#include "..\objects\Bullets.h"
#include "..\utils\background_grid.h"

const static float SHOOT_TTL = 0.2f;
const static float SPAWN_QUEUE_TTL = 0.2f;
const static float SPAWN_DELAY = 5.0f;



// ---------------------------------------------------------------
// circle spawn function
// ---------------------------------------------------------------
void spawnCircle(GameContext* ctx, SpawnItem* items, int count, int type) {
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
void spawnLeftLine(GameContext* ctx, SpawnItem* items, int count, int type) {
	int cnt = 0;
	int start = count * 2;
	for (int i = 0; i < count; ++i) {
		SpawnItem& item = items[cnt++];
		ds::vec2 p = ctx->grid.convert(-2 + i, start - i * 2);
		item.pos = p;
		item.type = type;
	}
}

// ---------------------------------------------------------------
// right line spawn function
// ---------------------------------------------------------------
void spawnRightLine(GameContext* ctx, SpawnItem* items, int count, int type) {
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
void spawnTopLine(GameContext* ctx, SpawnItem* items, int count, int type) {
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
void spawnBottomLine(GameContext* ctx, SpawnItem* items, int count, int type) {
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
MainState::MainState(GameContext* ctx, BackgroundState* backgroundState) : ArenaGameState(ctx, "MainState"), _backgroundState(backgroundState) {
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

	sprites::create(_ctx->sprites, ds::vec2(10, 10), ds::vec4(120, 40, 20, 20), SpriteType::DUMMY);
	sprites::create(_ctx->sprites, ds::vec2(10, 854), ds::vec4(120, 40, 20, 20), SpriteType::DUMMY);
	sprites::create(_ctx->sprites, ds::vec2(1526, 10), ds::vec4(120, 40, 20, 20), SpriteType::DUMMY);
	sprites::create(_ctx->sprites, ds::vec2(1526, 854), ds::vec4(120, 40, 20, 20), SpriteType::DUMMY);

}

MainState::~MainState() {
}

int MainState::tick(float dt, EventStream* stream) {
	
	objects::preapre_updates(_ctx->sprites);

	grid::tick(&_ctx->grid_context, dt);

	if (_running) {
		movePlayer(dt);
		handleShooting();
		moveBullets(dt);
		moveEnemies(dt);
		handleCollisions();
		handlePlayerCollision(stream);
	}
	
	if (_spawning) {
		spawn(dt);
	}
	
		
	spawnEnemies(dt);

	if (!_running) {
		_killTimer += dt;
		if (_killTimer >= 0.4f) {
			_killTimer -= 0.4f;
			SUID enemy = enemies::get_first(_ctx);
			if (enemy != INVALID_SUID) {
				ds::vec2 pos = sprites::get_position(_ctx->sprites, enemy);
				_ctx->particleManager->emitt(_ctx->enemyExplosion, pos, _ctx->emitterSettings[PSystems::PS_EXPLOSION]);
				_ctx->particleManager->emitt(_ctx->enemyExplosion, pos, _ctx->emitterSettings[PSystems::PS_LIGHT_STREAKS]);
				sprites::remove(_ctx->sprites, enemy);
			}
		}
	}
	_ctx->particleManager->tick(dt);

	objects::apply_forces(_ctx->sprites, dt);

	if (ds::isMouseButtonPressed(1) && !_rightClick) {
		_rightClick = true;
	}
	if (!ds::isMouseButtonPressed(1) && _rightClick) {
		_rightClick = false;
	}

	if (_rightClick) {
		ds::vec2 mp = objects::convert_mouse_to_world(_ctx);
		_selected = objects::get_selected_object(_ctx, mp, 20.0f);
	}
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
	sprites::clear(_ctx->sprites);
	player::create(_ctx, ds::vec2(960, 540));
	sprites::set_energy(_ctx->sprites, _ctx->player_id, 10);
	_active = true;
	_running = true;
	_spawning = false;
	_rightClick = false;
	_selected = INVALID_SUID;
}

void MainState::render(SpriteBatchBuffer* buffer) {
	_ctx->grid_context.screen_center = _ctx->world_pos;
	grid::render(&_ctx->grid_context);

	// particles
	buffer->flush();	
	buffer->setScreenCenter(ds::vec2(640,360));
	ds::vec2 mp = ds::getMousePosition();
	if (_running) {
		int player_energy = sprites::get_energy(_ctx->sprites, _ctx->player_id);
		numbers::draw(buffer, ds::vec2(120, 680), player_energy * 10, 3, false);
		numbers::draw(buffer, ds::vec2(1050, 680), _ctx->score, 6, true);
		// cursor
		buffer->add(mp, ds::vec4(160, 40, 20, 20));
	}
	buffer->flush();
	buffer->setScreenCenter(_ctx->world_pos);
	_ctx->particleManager->setScreenCenter(_ctx->world_pos);
	_ctx->particleManager->render();
	
	buffer->setScreenCenter(_ctx->world_pos);

	sprites::render(_ctx->sprites, buffer);
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
		_spawnFunctions[f](_ctx, items, 6, t);
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
			enemies::create(_ctx, np, type);
			_ctx->particleManager->emitt(_ctx->wakeUpSystem, np, _ctx->emitterSettings[PSystems::PS_WAKEUP]);
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
		float freq = 1.0f / _ctx->settings.fire_rate;
		_shootingTimer += static_cast<float>(ds::getElapsedSeconds());
		if (_shootingTimer >= freq) {
			_shootingTimer -= freq;
			bullets::add(_ctx, 20.0f,  10.0f);
			bullets::add(_ctx, 20.0f, -10.0f);
		}
	}
}

// ---------------------------------------------------------------
// handle player collisions
// ---------------------------------------------------------------
bool MainState::handlePlayerCollision(EventStream* stream) {

	ds::vec2 player_pos = player::get_position(_ctx);
	SUID enemy = enemies::check_collision(_ctx, player_pos, 20.0f);
	if (enemy != INVALID_SUID) {
		ds::vec2 enemy_pos = sprites::get_position(_ctx->sprites, enemy);
		_ctx->particleManager->emitt(_ctx->enemyExplosion, enemy_pos, _ctx->emitterSettings[PSystems::PS_EXPLOSION]);
		_ctx->particleManager->emitt(_ctx->enemyExplosion, enemy_pos, _ctx->emitterSettings[PSystems::PS_LIGHT_STREAKS]);
		sprites::remove(_ctx->sprites, enemy);
		// FIXME: remove player
		_ctx->particleManager->emitt(_ctx->enemyExplosion, player_pos, _ctx->emitterSettings[PSystems::PS_DEATH]);
		stream->add(ET_PLAYER_KILLED);
	}
	return false;
}

// ---------------------------------------------------------------
// handle collisions
// ---------------------------------------------------------------
void MainState::handleCollisions() {
	float dist = 0.0f;
	ds::vec2 pnv;
	uint32_t cnt = 0;
	bool hit = false;

	ds::vec2* positions = _ctx->sprites.positions;
	SpriteType::Enum* types = _ctx->sprites.types;
	SUID* suids = _ctx->sprites.ids;
	int counter = 0;
	for (uint32_t i = 0; i < _ctx->sprites.num; ++i) {
		if (*types == SpriteType::BULLET) {
			SUID enemy = enemies::check_collision(_ctx, *positions, 5.0f);
			if ( enemy != INVALID_SUID) {
				_ctx->particleManager->emitt(_ctx->enemyExplosion, *positions, _ctx->emitterSettings[PSystems::PS_BULLET]);
				sprites::remove(_ctx->sprites, *suids);
				// FIXME: check energy
				ds::vec2 enemy_pos = sprites::get_position(_ctx->sprites, enemy);
				_ctx->particleManager->emitt(_ctx->enemyExplosion, enemy_pos, _ctx->emitterSettings[PSystems::PS_EXPLOSION]);
				_ctx->particleManager->emitt(_ctx->enemyExplosion, enemy_pos, _ctx->emitterSettings[PSystems::PS_LIGHT_STREAKS]);
				sprites::remove(_ctx->sprites, enemy);
				_ctx->score += 50;
			}
			else {
				++positions;
				++types;
				++suids;
			}
		}
		else {
			++positions;
			++types;
			++suids;
		}
	}
}
// ---------------------------------------------------------------
// move bullets
// ---------------------------------------------------------------
void MainState::moveBullets(float dt) {
	ds::vec2 tmp[32];
	int num = bullets::move(_ctx->sprites, dt, _backgroundState, _ctx->world_size, tmp, 32);
	for (int i = 0; i < num; ++i) {
		_ctx->particleManager->emitt(_ctx->enemyExplosion, tmp[i], _ctx->emitterSettings[PSystems::PS_BULLET]);
	}
}

// ---------------------------------------------------------------
// move enemies
// ---------------------------------------------------------------
void MainState::moveEnemies(float dt) {

	enemies::look_at_player(_ctx);

	enemies::seek_player(_ctx);

	enemies::separate(_ctx);
	
}

// ---------------------------------------------------------------
// move player
// ---------------------------------------------------------------
void MainState::movePlayer(float dt) {
	player::move(_ctx, dt);
	player::move_camera(_ctx, dt);
	ds::vec2 player_pos = player::get_position(_ctx);
	_backgroundState->highlight(player_pos, _ctx->settings.playerHightlightColor);
	_backgroundState->setScreenCenter(_ctx->world_pos);
}

// ---------------------------------------------------------------
// render GUI
// ---------------------------------------------------------------
void MainState::renderGUI() {
	int state = 1;
	if (gui::begin("Debug", &state)) {
		ds::vec2 player_pos = player::get_position(_ctx);
		int px = player_pos.x;
		int py = player_pos.y;
		gui::Value("Player", ds::vec2(px, py));
		px = _ctx->world_pos.x;
		py = _ctx->world_pos.y;
		gui::Value("World", ds::vec2(px, py));
		ds::vec2 mp = ds::getMousePosition();
		px = mp.x;
		py = mp.y;
		gui::Value("Mouse", ds::vec2(px, py));
		mp = objects::convert_mouse_to_world(_ctx);
		px = mp.x;
		py = mp.y;
		gui::Value("World Mouse", ds::vec2(px, py));
		Hex h = _ctx->grid.convertFromMousePos();
		gui::Value("q", h.q);
		gui::Value("r", h.r);
		if (_spawning) {
			if (gui::Button("Stop spawning")) {
				_spawning = false;
			}
		}
		else {
			if (gui::Button("Start spawning")) {
				startSpawning();
			}
		}
		if (gui::Button("Spawn left")) {
			SpawnItem items[32];
			_spawnFunctions[1](_ctx, items, 8, 0);
			for (int i = 0; i < 8; ++i) {
				_spawnItems.push(items[i]);
			}
		}
		if (gui::Button("Force")) {
			grid::apply_force(&_ctx->grid_context, 5, 5, ds::vec2(50, 0));
		}
		if (gui::Button("Force 2")) {
			ds::vec2 player_pos = player::get_position(_ctx);
			grid::apply_force(&_ctx->grid_context, player_pos, 40.0f, 40.0f);
		}
		gui::Value("Sprites", _ctx->sprites.num);
		gui::Checkbox("Running", &_running);
		if (_selected != INVALID_SUID) {
			uint32_t idx = sprites::get_index(_ctx->sprites, _selected);
			gui::Value("SUID", _selected);
			gui::Value("Index", idx);
			gui::Value("Position", sprites::get_position(_ctx->sprites, _selected));
			gui::Value("Energy", sprites::get_energy(_ctx->sprites, _selected));
			gui::Value("Rotation", _ctx->sprites.rotations[idx] / ds::TWO_PI * 360.0f);
			gui::Value("Scale", _ctx->sprites.scales[idx]);
		}
	}
}

