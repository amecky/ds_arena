#include "SpriteEditorState.h"
#include <ds_imgui.h>

const static char* NAMES[] = { 
	"Item 1","Item 2","Item 3","Item 4","Item 5","Item 6","Item 7","Item 8","Item 9",
	"Item10","Item 11","Item 12","Item 13","Item 14","Item 15","Item 16","Item 17","Item 18","Item 19",
	"Item20","Item 21","Item 22","Item 23","Item 24","Item 25","Item 26","Item 27","Item 28","Item 29",
	"Item30","Item31","Item32","Item33"
};

SpriteEditorState::SpriteEditorState(GameContext* ctx) : ArenaGameState(ctx, "SpriteEditorState") {
	RID textureID = ds::findResource(SID("content\\TextureArray.png"),ds::ResourceType::RT_SRV);
	//ds::BlendStateInfo blendInfo = { ds::BlendStates::ONE, ds::BlendStates::ONE, ds::BlendStates::ONE, ds::BlendStates::ONE, true };
	ds::BlendStateInfo blendInfo = { ds::BlendStates::ONE, ds::BlendStates::SRC_ALPHA, ds::BlendStates::ONE, ds::BlendStates::INV_SRC_ALPHA, true };
	RID bs_id = ds::createBlendState(blendInfo);
	SpriteBatchBufferInfo sbbInfo = { 2048, textureID, ds::TextureFilters::POINT, bs_id};
	_sprites = new SpriteBatchBuffer(sbbInfo);

	_dialogState = 1;
	clear();
	sprintf_s(_fileName, "%s", "data.bin");
	sprintf_s(_message, "-");
}

SpriteEditorState::~SpriteEditorState() {
	delete _sprites;
}

// -------------------------------------------------------
// clear internal list
// -------------------------------------------------------
void SpriteEditorState::clear() {
	_num = 0;
	_selection = -1;
	_spriteSelectionOffset = 0;
	_spriteSelectionState = 1;

	for (int i = 0; i < 16; ++i) {
		_items[i].position = ds::vec2(0, 0);
		_items[i].scaling = ds::vec2(1, 1);
		_items[i].rotation = 0.0f;
		_items[i].color = ds::Color(255, 255, 255, 255);
		//_items[i].textureRect = ds::vec4(330, 70, 50, 48);
		_items[i].textureRect = ds::vec4(200, 20, 40, 40);
	}
}

// -------------------------------------------------------
// load sprites
// -------------------------------------------------------
void SpriteEditorState::load() {
	clear();
	FILE* fp = fopen(_fileName, "rb");
	if (fp) {
		fread(&_num, sizeof(int), 1, fp);
		for (int i = 0; i < _num; ++i) {
			fread(&_items[i], sizeof(Sprite), 1, fp);
		}
		fclose(fp);
	}
}

// -------------------------------------------------------
// save sprites
// -------------------------------------------------------
void SpriteEditorState::save() {
	FILE* fp = fopen(_fileName, "wb");
	if (fp) {
		fwrite(&_num, sizeof(int), 1, fp);
		for (int i = 0; i < _num; ++i) {
			fwrite(&_items[i], sizeof(Sprite), 1, fp);
		}
		fclose(fp);
	}
}

int SpriteEditorState::tick(float dt, EventStream* stream) {
	return 0;
}

// -------------------------------------------------------
// render
// -------------------------------------------------------
void SpriteEditorState::render(SpriteBatchBuffer* buffer) {
	gui::start(ds::vec2(0, 730));
	gui::begin("Settings", &_dialogState);
	if (_dialogState == 1) {
		gui::Input("Name", _fileName, 16);
		gui::beginGroup();
		if (gui::Button("Load")) {			
			load();
			sprintf_s(_message, "File %s loaded",_fileName);
		}
		if (gui::Button("Save")) {
			save();
			sprintf_s(_message, "File %s saved", _fileName);
		}
		if (gui::Button("Clear")) {
			clear();
			sprintf_s(_message, "All sprites cleared");
		}
		gui::endGroup();
		gui::DropDownBox("Sprites", NAMES, _num, &_spriteSelectionState, &_selection, &_spriteSelectionOffset, 6);
		gui::beginGroup();
		if (gui::Button("Add")) {
			if (_num < 32) {
				++_num;
				sprintf_s(_message, "New sprite added");
			}
		}		
		if (gui::Button("Remove")) {
			if (_selection != -1) {
				if (_num > 1) {
					for (int i = _selection; i < (_num - 1); ++i) {
						_items[i] = _items[i + 1];
					}
				}
				sprintf_s(_message, "Sprite removed");
				--_num;
			}
		}
		gui::endGroup();
	}	
	if (_selection != -1) {
		int state = 1;
		gui::begin(NAMES[_selection], &state);
		Sprite& s = _items[_selection];
		gui::Input("Pos", &s.position);
		gui::Input("Scaling", &s.scaling);
		float r = s.rotation / (ds::PI * 2.0f) * 360.0f;
		gui::Input("Rotation", &r);
		s.rotation = r / 360.0f * ds::PI * 2.0f;
		gui::Input("Color", &s.color);
	}
	gui::Message(_message);
	gui::end();

	_sprites->begin();

	for (int i = 0; i < _num; ++i) {
		const Sprite& s = _items[i];
		_sprites->add(ds::vec2(512, 384) + s.position, s.textureRect, s.scaling, s.rotation, s.color);
	}

	_sprites->flush();
	
}