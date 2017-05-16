#include <diesel.h>
#include "..\lib\EventStream.h"
#include "..\utils\GameContext.h"
#include "..\utils\SpriteBatchBuffer.h"
// ---------------------------------------------------------------
// GameState
// ---------------------------------------------------------------
class GameState {

public:
	GameState(GameContext* ctx, const char* name) : _ctx(ctx), _active(false) {
		_hash = ds::StaticHash(name);
	}
	virtual ~GameState() {}
	virtual int tick(float dt, EventStream* stream) = 0;
	virtual void render(SpriteBatchBuffer* buffer) = 0;
	virtual void activate() = 0;
	virtual void deactivate() = 0;
	const ds::StaticHash& getHash() const {
		return _hash;
	}
	const bool isActive() const {
		return _active;
	}
protected:
	GameContext* _ctx;
	bool _active;
	ds::StaticHash _hash;
};

// ---------------------------------------------------------------
// StateMachine
// ---------------------------------------------------------------
class StateMachine {

public:
	StateMachine(SpriteBatchBuffer* buffer);
	~StateMachine();
	void add(GameState* state);
	void activate(const char* name);
	void deactivate(const char* name);
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
	SpriteBatchBuffer* _buffer;
};