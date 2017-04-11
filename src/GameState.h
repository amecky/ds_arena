#pragma once
#include "utils\Stack.h"
#include "utils\EventStream.h"
#include "utils\HexGrid.h"
#include "ElasticBorder.h"

enum EventTypes {
	ET_NONE,
	ET_PREPARE_ELAPSED
};

// ---------------------------------------------------------------
// GameState
// ---------------------------------------------------------------
class GameState {

public:
	GameState(const char* name) : _active(false) {
		_hash = ds::StaticHash(name);
	}
	virtual ~GameState() {}
	virtual int tick(float dt, EventStream* stream) = 0;
	virtual void render() = 0;
	virtual void activate() = 0;
	virtual void deactivate() = 0;
	const ds::StaticHash& getHash() const {
		return _hash;
	}
	const bool isActive() const {
		return _active;
	}
protected:
	bool _active;
	ds::StaticHash _hash;
};

// ---------------------------------------------------------------
// PrepareState
// ---------------------------------------------------------------
class PrepareState : public GameState {

public:
	PrepareState() : GameState("PrepareState") {}
	int tick(float dt, EventStream* stream);
	void render();
	void activate();
	void deactivate();
private:
	float _prepareTimer;
};

// ---------------------------------------------------------------
// GameOverState
// ---------------------------------------------------------------
class GameOverState : public GameState {

public:
	GameOverState() : GameState("GameOverState") {}
	int tick(float dt, EventStream* stream);
	void render();
	void activate() {
		_active = true;
	}
	void deactivate() {
		_active = false;
	}
};
// ---------------------------------------------------------------
// BackgroundState
// ---------------------------------------------------------------
class HexGrid;

class BackgroundState : public GameState {

public:
	BackgroundState();
	~BackgroundState();
	int tick(float dt, EventStream* stream);
	void render();
	void activate() {
		_active = true;
	}
	void deactivate() {
		_active = false;
	}
	Hex convert(const ds::vec2& p) {
		return _grid.convert(p);
	}
	ds::vec2 convert(const Hex& h) {
		return _grid.convert(h);
	}
	void highlight(const ds::vec2& p, const ds::Color& color);
	bool borderCollision(const ds::vec2& p, float radius);
private:
	HexGrid _grid;
	int _height;
	int _width;
	ElasticBorderSettings _borderSettings;
	ElasticBorder* _borders;
};

// ---------------------------------------------------------------
// GameStateManager
// ---------------------------------------------------------------
class GameStateManager {

public:
	GameStateManager();
	~GameStateManager();
	void add(GameState* state);
	void activate(const char* name);
	void deactivate(const char* name);
	void connect(const char* first, const char* second, int outcome);
	void tick(float dt);
	void render();
	bool hasEvents() const;
	uint32_t numEvents() const;
	const bool getEvent(uint32_t index, void* p) const;
	const int getEventType(uint32_t index) const;
	const bool containsEventType(uint32_t type) const;
private:
	GameState* find(const char* name);
	std::vector<GameState*> _states;
	EventStream _stream;
};