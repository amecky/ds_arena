#include "GameState.h"
#include "..\utils\sprites.h"
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
	if (_prepareTimer >= 5.0f) {
		stream->add(ET_PREPARE_ELAPSED);
	}
	return 0;
}

void PrepareState::render() {
	sprites::add(ds::vec2(512, 384), ds::vec4(80, 160, 370, 60));
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

void GameOverState::render() {
	sprites::add(ds::vec2(512, 580), ds::vec4(0, 480, 395, 60));
	sprites::add(ds::vec2(512, 220), ds::vec4(0, 360, 300, 56));
	sprites::add(ds::vec2(512, 120), ds::vec4(0, 300, 300, 56));
	// score
	sprites::add(ds::vec2(512, 480), ds::vec4(0, 620, 140, 40));
	numbers::draw(ds::vec2(420, 420), _ctx->score, 6);
	// rank
	sprites::add(ds::vec2(512, 360), ds::vec4(145, 620, 125, 40));
	if (_rank >= 0) {
		numbers::draw(ds::vec2(500, 300), _rank + 1, 2);
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

void HighscoreState::render() {
	sprites::add(ds::vec2(512, 580), ds::vec4(518, 185, 420, 84),ds::vec2(1.0f),0.0f,ds::Color(182,255,134,255));
	sprites::add(ds::vec2(512, 120), ds::vec4(0, 300, 300, 56));
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
		numbers::draw(ds::vec2(380, 500 - cnt * 70), i + 1, 2, false, ds::Color(1.0f,1.0f,1.0f,a));
		// score
		numbers::draw(ds::vec2(470, 500 - cnt * 70), _ctx->highscores[i], 6, true, ds::Color(1.0f, 1.0f, 1.0f, a));
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

void MainMenuState::render() {
	sprites::add(ds::vec2(512, 580), ds::vec4(550, 104, 365, 84),ds::vec2(1.0f),0.0f,ds::Color(5,248,205,255));
	sprites::add(ds::vec2(512, 400), ds::vec4(0, 240, 300, 56));
	sprites::add(ds::vec2(512, 200), ds::vec4(0, 300, 300, 56));
	sprites::add(ds::vec2(512, 300), ds::vec4(0, 420, 300, 56));
}

// ---------------------------------------------------------------
// Background state
// ---------------------------------------------------------------
BackgroundState::BackgroundState(GameContext* ctx) : GameState(ctx, "BackgroundState") {
	_width = 26;
	_height = 18;
	_grid.resize(_width, _height);

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

void BackgroundState::render() {
	sprites::begin();
	// background
	for (int r = 0; r < _height; r++) {
		int q_offset = r >> 1;
		for (int q = -q_offset; q < _width - q_offset; q++) {
			Hex h = Hex(q, r);
			if (_grid.isValid(h)) {
				GridItem& current = _grid.get(h);
				if (current.timer > 0.0f) {
					sprites::add(current.position, ds::vec4(320, 0, 42, 46), ds::vec2(1, 1), 0.0f, current.color);
				}
				else {
					sprites::add(current.position, ds::vec4(380, 0, 42, 46), ds::vec2(1, 1), 0.0f, current.color);
				}
			}
		}
	}
	sprites::flush();
	// elastic borders
	_borders->render();
	sprites::begin();
}
