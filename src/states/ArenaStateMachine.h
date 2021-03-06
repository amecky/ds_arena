#pragma once
#include <diesel.h>
#include <StateMachine.h>
#include "..\utils\GameContext.h"
#include <SpriteBatchBuffer.h>
// ---------------------------------------------------------------
// GameState
// ---------------------------------------------------------------
class ArenaGameState : public GameState {

public:
	ArenaGameState(GameContext* ctx, const char* name) : GameState(name) , _ctx(ctx) {}
	virtual ~ArenaGameState() {}
	virtual void render() {}
	virtual void render(SpriteBatchBuffer* buffer) = 0;
	virtual void renderGUI() {}
protected:
	GameContext* _ctx;
};

// ---------------------------------------------------------------
// StateMachine
// ---------------------------------------------------------------
class ArenaStateMachine : public StateMachine {

public:
	ArenaStateMachine(SpriteBatchBuffer* buffer);
	virtual ~ArenaStateMachine() {}
	void render();
	void renderGUI();
private:
	SpriteBatchBuffer* _buffer;
};