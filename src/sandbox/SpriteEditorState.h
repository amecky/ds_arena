#pragma once
#include "..\states\GameState.h"


class SpriteEditorState : public ArenaGameState {

public:
	SpriteEditorState(GameContext* ctx);
	~SpriteEditorState();
	int tick(float dt, EventStream* stream);
	void render(SpriteBatchBuffer* buffer);
	void activate() {
		_active = true;
	}
	void deactivate() {
		_active = false;
	}
private:
	void clear();
	void load();
	void save();
	int _dialogState;
	SpriteBatchBuffer* _sprites;
	int _num;
	Sprite _items[32];
	int _selection;
	int _spriteSelectionState;
	int _spriteSelectionOffset;
	char _fileName[16];
	char _message[32];
};