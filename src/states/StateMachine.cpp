#include "StateMachine.h"
#include "..\sprites.h"
// ---------------------------------------------------------------
// GameStateManager
// ---------------------------------------------------------------
StateMachine::StateMachine() {
	_stream.reset();
}

StateMachine::~StateMachine() {

}

void StateMachine::tick(float dt) {
	_stream.reset();
	for (size_t i = 0; i < _states.size(); ++i) {
		if (_states[i]->isActive()) {
			_states.at(i)->tick(dt, &_stream);
		}
	}

}

GameState* StateMachine::find(const char* name) {
	ds::StaticHash h(name);
	for (size_t i = 0; i < _states.size(); ++i) {
		if (_states[i]->getHash().get() == h.get()) {
			return _states[i];
		}
	}
	return 0;
}

void StateMachine::activate(const char* name) {
	GameState* state = find(name);
	if (state != 0) {
		state->activate();
	}
}

void StateMachine::deactivate(const char* name) {
	GameState* state = find(name);
	if (state != 0) {
		state->deactivate();
	}
}

void StateMachine::render() {
	sprites::begin();
	for (size_t i = 0; i < _states.size(); ++i) {
		if (_states[i]->isActive()) {
			_states.at(i)->render();
		}
	}
	sprites::flush();
}

void StateMachine::add(GameState* state) {
	_states.push_back(state);
}

bool StateMachine::hasEvents() const {
	return _stream.num() > 0;
}

uint32_t StateMachine::numEvents() const {
	return _stream.num();

}

const bool StateMachine::getEvent(uint32_t index, void* p) const {
	return _stream.get(index, p);
}

const int StateMachine::getEventType(uint32_t index) const {
	return _stream.getType(index);
}

const bool StateMachine::containsEventType(uint32_t type) const {
	return _stream.containsType(type);
}