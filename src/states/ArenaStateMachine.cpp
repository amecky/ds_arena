#include "ArenaStateMachine.h"

// ---------------------------------------------------------------
// GameStateManager
// ---------------------------------------------------------------
ArenaStateMachine::ArenaStateMachine(SpriteBatchBuffer* buffer) : StateMachine() , _buffer(buffer) {	
}

void ArenaStateMachine::render() {
	_buffer->begin();
	for (size_t i = 0; i < _states.size(); ++i) {
		if (_states[i]->isActive()) {
			ArenaGameState* ags = (ArenaGameState*)_states.at(i);
			ags->render(_buffer);
		}
	}
	_buffer->flush();
}
