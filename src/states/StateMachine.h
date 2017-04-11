#include "..\..\..\diesel\diesel.h"
#include "..\utils\EventStream.h"

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
// StateMachine
// ---------------------------------------------------------------
class StateMachine {

public:
	StateMachine();
	~StateMachine();
	void add(GameState* state);
	void activate(const char* name);
	void deactivate(const char* name);
	//void connect(const char* first, const char* second, int outcome);
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