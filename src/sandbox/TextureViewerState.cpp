#include "TextureViewerState.h"
#include "..\states\ArenaStateMachine.h"

int TextureViewerState::tick(float dt, EventStream * stream) {
	return 0;
}

void TextureViewerState::render(SpriteBatchBuffer * buffer) {
	buffer->add(ds::vec2(512, 384), ds::vec4(0, 0, 1024, 1024));
}

void TextureViewerState::activate() {
	_active = true;
}
