#include "GameState.h"
#include "sprites.h"
#include "utils\HexGrid.h"
#include "utils\json.h"

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
	return 0;
}

void GameOverState::render() {
	sprites::add(ds::vec2(512, 500), ds::vec4(0, 480, 395, 60));
	sprites::add(ds::vec2(512, 300), ds::vec4(0, 360, 240, 54));
	sprites::add(ds::vec2(512, 200), ds::vec4(0, 300, 240, 54));
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

// ---------------------------------------------------------------
// GameStateManager
// ---------------------------------------------------------------
GameStateManager::GameStateManager() {
	_stream.reset();
}

GameStateManager::~GameStateManager() {

}

void GameStateManager::tick(float dt) {
	_stream.reset();
	for (size_t i = 0; i < _states.size(); ++i) {
		if (_states[i]->isActive()) {
			_states.at(i)->tick(dt, &_stream);
		}
	}

}

GameState*  GameStateManager::find(const char* name) {
	ds::StaticHash h(name);
	for (size_t i = 0; i < _states.size(); ++i) {
		if (_states[i]->getHash().get() == h.get()) {
			return _states[i];
		}
	}
	return 0;
}

void GameStateManager::activate(const char* name) {
	GameState* state = find(name);
	if (state != 0) {
		state->activate();
	}
}

void GameStateManager::deactivate(const char* name) {
	GameState* state = find(name);
	if (state != 0) {
		state->deactivate();
	}
}

void GameStateManager::render() {
	sprites::begin();
	for (size_t i = 0; i < _states.size(); ++i) {
		if (_states[i]->isActive()) {
			_states.at(i)->render();
		}
	}
	sprites::flush();
}

void GameStateManager::add(GameState* state) {
	_states.push_back(state);
}

bool GameStateManager::hasEvents() const {
	return _stream.num() > 0;
}

uint32_t GameStateManager::numEvents() const {
	return _stream.num();

}

const bool GameStateManager::getEvent(uint32_t index, void* p) const {
	return _stream.get(index, p);
}

const int GameStateManager::getEventType(uint32_t index) const {
	return _stream.getType(index);
}

const bool GameStateManager::containsEventType(uint32_t type) const {
	return _stream.containsType(type);
}