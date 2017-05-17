#include "GameState.h"
#include "..\utils\json.h"
#include "..\utils\hud.h"
#include "..\utils\highscores.h"

// -------------------------------------------------------
// check if mouse cursor is inside box
// -------------------------------------------------------
static bool isCursorInside(const ds::vec2& p, const ds::vec2& dim) {
	ds::vec2 mp = ds::getMousePosition();
	if (mp.x < (p.x - dim.x * 0.5f)) {
		return false;
	}
	if (mp.x >(p.x + dim.x * 0.5f)) {
		return false;
	}
	if (mp.y < (p.y - dim.y * 0.5f)) {
		return false;
	}
	if (mp.y >(p.y + dim.y * 0.5f)) {
		return false;
	}
	return true;
}

// ---------------------------------------------------------------
// Prepare state
// ---------------------------------------------------------------
int PrepareState::tick(float dt, EventStream* stream) {
	_prepareTimer += dt;
	if (_prepareTimer >= _ctx->settings.prepareTTL) {
		stream->add(ET_PREPARE_ELAPSED);
	}
	return 0;
}

void PrepareState::render(SpriteBatchBuffer* buffer) {
	float s = 1.0f;
	if (_prepareTimer <= _ctx->settings.prepareFlashingTTL) {
		float norm = _prepareTimer / _ctx->settings.prepareFlashingTTL;
		s = _scalePath.get(norm);
	}
	buffer->add(ds::vec2(512, 384), ds::vec4(544, 274, 380, 70),ds::vec2(s,s),0.0f,ds::Color(17,236,228,255));
}

void PrepareState::activate() {
	_prepareTimer = 0.0f;
	_active = true;
}

void PrepareState::deactivate() {
	_prepareTimer = 0.0f;
	_active = false;
}

// ---------------------------------------------------------------
// Game over state
// ---------------------------------------------------------------
void GameOverState::activate() {
	_active = true;
	_rank = highscore::rank(_ctx, _ctx->score);
}

int GameOverState::tick(float dt, EventStream* stream) {
	if (ds::isMouseButtonClicked(0)) {
		if (isCursorInside(ds::vec2(512, 220), ds::vec2(300, 56))) {
			stream->add(ET_GAME_OVER_PLAY);
			return 1;
		}
		if (isCursorInside(ds::vec2(512, 120), ds::vec2(300, 56))) {
			stream->add(ET_GAME_OVER_EXIT);
			return 2;
		}
	}
	return 0;
}

void GameOverState::render(SpriteBatchBuffer* buffer) {
	buffer->add(ds::vec2(512, 580), ds::vec4(532, 28, 400, 70),ds::vec2(1.0f),0.0f,ds::Color(255,0,0,255));
	buffer->add(ds::vec2(512, 220), ds::vec4(0, 360, 300, 56));
	buffer->add(ds::vec2(512, 120), ds::vec4(0, 300, 300, 56));
	// score
	buffer->add(ds::vec2(512, 470), ds::vec4(80, 120, 124, 40));
	numbers::draw(buffer,ds::vec2(440, 420), _ctx->score, 6);
	// rank
	buffer->add(ds::vec2(512, 350), ds::vec4(212, 120, 110, 40));
	if (_rank >= 0) {
		numbers::draw(buffer,ds::vec2(500, 300), _rank + 1, 2);
	}
}

// ---------------------------------------------------------------
// highscore state
// ---------------------------------------------------------------
int HighscoreState::tick(float dt, EventStream* stream) {
	_timer += dt;
	if (_timer >= 2.0f) {
		_timer -= 2.0f;
		_index += 5;
		_index = _index & 5;
	}
	if (ds::isMouseButtonClicked(0)) {
		if (isCursorInside(ds::vec2(512, 120), ds::vec2(300, 56))) {
			stream->add(ET_HIGHSCORES_EXIT);
			return 2;
		}
	}
	return 0;
}

void HighscoreState::render(SpriteBatchBuffer* buffer) {
	buffer->add(ds::vec2(512, 580), ds::vec4(518, 185, 420, 84),ds::vec2(1.0f),0.0f,ds::Color(182,255,134,255));
	buffer->add(ds::vec2(512, 120), ds::vec4(0, 300, 300, 56));
	int cnt = 0;
	float norm = _timer / 2.0f;
	float a = 1.0f;
	if (norm < 0.25f) {
		a = norm / 0.25f;
	}
	if (norm > 0.75f) {
		a = 1.0f - norm / 0.75f;
	}
	for (int i = _index; i < (_index + 5); ++i) {
		numbers::draw(buffer, ds::vec2(380, 500 - cnt * 70), i + 1, 2, false, ds::Color(1.0f,1.0f,1.0f,a));
		// score
		numbers::draw(buffer, ds::vec2(470, 500 - cnt * 70), _ctx->highscores[i], 6, true, ds::Color(1.0f, 1.0f, 1.0f, a));
		++cnt;
	}
}

// ---------------------------------------------------------------
// MainMenuState
// ---------------------------------------------------------------
int MainMenuState::tick(float dt, EventStream* stream) {
	if (ds::isMouseButtonClicked(0)) {
		if (isCursorInside(ds::vec2(512, 400), ds::vec2(300, 56))) {
			stream->add(ET_MAIN_MENU_PLAY);
			return 1;
		}
		if (isCursorInside(ds::vec2(512, 200), ds::vec2(300, 56))) {
			stream->add(ET_MAIN_MENU_EXIT);
			return 2;
		}
		if (isCursorInside(ds::vec2(512, 300), ds::vec2(300, 56))) {
			stream->add(ET_MAIN_MENU_HIGHSCORES);
			return 2;
		}
	}
	return 0;
}

void MainMenuState::render(SpriteBatchBuffer* buffer) {
	buffer->add(ds::vec2(512, 580), ds::vec4(550, 104, 365, 84),ds::vec2(1.0f),0.0f,ds::Color(5,248,205,255));
	buffer->add(ds::vec2(512, 400), ds::vec4(0, 240, 300, 56));
	buffer->add(ds::vec2(512, 200), ds::vec4(0, 300, 300, 56));
	buffer->add(ds::vec2(512, 300), ds::vec4(0, 420, 300, 56));
}

// ---------------------------------------------------------------
// Background state
// ---------------------------------------------------------------
BackgroundState::BackgroundState(GameContext* ctx) : GameState(ctx, "BackgroundState") {
	_width = 26;
	_height = 18;
	_grid.resize(_width, _height);
	_grid.setBaseColor(ctx->settings.gridBaseColor);
	SJSONReader settingsReader;
	settingsReader.parse("content\\settings.json");
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
	RID textureID = ds::findResource(SID("content\\TextureArray.png"), ds::ResourceType::RT_SRV);
	_borderSettings.textureID = textureID;
	_borders = new ElasticBorder(&_borderSettings);
	_active = true;
}

BackgroundState::~BackgroundState() {
	delete _borders;
}

int BackgroundState::tick(float dt, EventStream* stream) {
	_grid.tick(dt);
	_borders->tick(dt);
	return 0;
}

void BackgroundState::highlight(const ds::vec2& p, const ds::Color& color) {
	_grid.highlight(p, color);
}

bool BackgroundState::borderCollision(const ds::vec2& p, float radius) {
	return _borders->collides(p, radius);
}

void BackgroundState::render(SpriteBatchBuffer* buffer) {
	// background
	for (int r = 0; r < _height; r++) {
		int q_offset = r >> 1;
		for (int q = -q_offset; q < _width - q_offset; q++) {
			Hex h = Hex(q, r);
			if (_grid.isValid(h)) {
				GridItem& current = _grid.get(h);
				if (current.timer > 0.0f) {
					buffer->add(current.position, ds::vec4(320, 0, 42, 46), ds::vec2(1, 1), 0.0f, current.color);
				}
				else {
					buffer->add(current.position, ds::vec4(380, 0, 42, 46), ds::vec2(1, 1), 0.0f, current.color);
				}
			}
		}
	}
	buffer->flush();
	// elastic borders
	_borders->render();
}


// ---------------------------------------------------------------
// ParticlesTestState
// ---------------------------------------------------------------
int ParticlesTestState::tick(float dt, EventStream* stream) {
	_timer += dt;
	if (_timer >= 1.0f) {
		_timer = 0.0f;
		if (_flags[0] == 1) {
			_ctx->particleManager->emitt(_ctx->enemyExplosion, ds::vec2(200, 384), _ctx->explosionSettings);
		}
		if (_flags[1] == 1) {
			_ctx->particleManager->emitt(_ctx->enemyExplosion, ds::vec2(500, 384), _ctx->bulletExplosionSettings);
		}
		if (_flags[2] == 1) {
			_ctx->particleManager->emitt(_ctx->enemyExplosion, ds::vec2(800, 384), _ctx->deathSettings);
		}
	}
	_ctx->particleManager->tick(dt);

	const char keys[] = { '1','2','3' };
	for (int i = 0; i < 3; ++i) {
		if (ds::isKeyPressed(keys[i])) {
			if (_pressed[i] == 0) {
				_flags[i] = (_flags[i] + 1) & 1;
				_pressed[i] = 1;
			}
		}
		else {
			_pressed[i] = 0;
		}
	}

	return 0;
}

void ParticlesTestState::render(SpriteBatchBuffer* buffer) {
	buffer->flush();
	_ctx->particleManager->render();
	for (int i = 0; i < 4; ++i) {
		ds::dbgPrint(0, 2 + i, "PS %d: %d", i, _flags[i]);
	}
}