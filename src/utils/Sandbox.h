#pragma once
#include <diesel.h>
#include <SpriteBatchBuffer.h>

class SandBox {

public:
	SandBox();
	~SandBox();
	void render();
	void tick(float elapsed);
private:
	int _dialogState;
	SpriteBatchBuffer* _sprites;
	int _num;
	Sprite _items[16];
	int _selection;
	int _spriteSelectionState;
	int _spriteSelectionOffset;
};