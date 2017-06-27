#pragma once
#include "..\states\GameState.h"

class TextureViewerState : public ArenaGameState {

public:
	TextureViewerState(GameContext* ctx) : ArenaGameState(ctx, "TextureViewerState") {}
	int tick(float dt, EventStream* stream);
	void render(SpriteBatchBuffer* buffer);
	void activate();
	void deactivate() {
		_active = false;
	}
private:
};