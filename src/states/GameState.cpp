#include "GameState.h"
#include "..\utils\sprites.h"
#include "..\utils\json.h"


bool renderButton(const ds::vec2& p, const ds::vec4& t) {
	sprites::add(p, t);
	return false;
}



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
int GameOverState::tick(float dt, EventStream* stream) {
	if (ds::isMouseButtonPressed(0)) {
		if (isCursorInside(ds::vec2(512, 300), ds::vec2(300, 56))) {
			stream->add(ET_GAME_OVER_PLAY);
			return 1;
		}
		if (isCursorInside(ds::vec2(512, 200), ds::vec2(300, 56))) {
			stream->add(ET_GAME_OVER_EXIT);
			return 2;
		}
	}
	return 0;
}

void GameOverState::render() {
	sprites::add(ds::vec2(512, 500), ds::vec4(0, 480, 395, 60));
	sprites::add(ds::vec2(512, 300), ds::vec4(0, 360, 300, 56));
	sprites::add(ds::vec2(512, 200), ds::vec4(0, 300, 300, 56));
}

// ---------------------------------------------------------------
// MainMenuState
// ---------------------------------------------------------------
int MainMenuState::tick(float dt, EventStream* stream) {
	if (ds::isMouseButtonPressed(0)) {
		if (isCursorInside(ds::vec2(512, 300), ds::vec2(300, 56))) {
			stream->add(ET_MAIN_MENU_PLAY);
			return 1;
		}
		if (isCursorInside(ds::vec2(512, 200), ds::vec2(300, 56))) {
			stream->add(ET_MAIN_MENU_EXIT);
			return 2;
		}
	}
	return 0;
}

void MainMenuState::render() {
	sprites::add(ds::vec2(512, 500), ds::vec4(0, 560, 346, 60));
	sprites::add(ds::vec2(512, 300), ds::vec4(0, 240, 300, 56));
	sprites::add(ds::vec2(512, 200), ds::vec4(0, 300, 300, 56));
}

// ---------------------------------------------------------------
// Background state
// ---------------------------------------------------------------
BackgroundState::BackgroundState() : GameState("BackgroundState") {
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
