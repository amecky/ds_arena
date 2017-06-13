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