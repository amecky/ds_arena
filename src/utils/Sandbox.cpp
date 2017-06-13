#include "Sandbox.h"
#include <ds_imgui.h>

const static char* NAMES[] = { "Item 1","Item 2","Item 3","Item 4","Item 5","Item 6","Item 7","Item 8","Item 9","Item10","Item 11","Item 12","Item 13","Item 14","Item 15","Item 16","Item 17","Item 18","Item 19" };
SandBox::SandBox() {
	RID textureID = ds::findResource(SID("content\\TextureArray.png"),ds::ResourceType::RT_SRV);
	ds::BlendStateInfo blendInfo = { ds::BlendStates::ONE, ds::BlendStates::ONE, ds::BlendStates::ONE, ds::BlendStates::ONE, true };
	RID bs_id = ds::createBlendState(blendInfo);
	SpriteBatchBufferInfo sbbInfo = { 2048, textureID, ds::TextureFilters::POINT, bs_id};
	_sprites = new SpriteBatchBuffer(sbbInfo);

	_dialogState = 1;
	_num = 0;
	_selection = -1;
	_spriteSelectionOffset = 0;
	_spriteSelectionState = 1;

	for (int i = 0; i < 16; ++i) {
		_items[i].position = ds::vec2(0, 0);
		_items[i].scaling = ds::vec2(1, 1);
		_items[i].rotation = 0.0f;
		_items[i].color = ds::Color(255, 255, 255, 255);
		_items[i].textureRect = ds::vec4(270, 70, 48, 48);
	}
}

SandBox::~SandBox() {
	delete _sprites;
}

void SandBox::render() {
	gui::start(ds::vec2(10, 700));
	gui::begin("Settings", &_dialogState);
	if (_dialogState == 1) {
		gui::beginGroup();
		if (gui::Button("Load")) {
			FILE* fp = fopen("data.bin", "rb");
			if (fp) {
				fread(&_num, sizeof(int), 1, fp);
				for (int i = 0; i < _num; ++i) {
					fread(&_items[i], sizeof(Sprite), 1, fp);
				}
				fclose(fp);
			}
		}
		if (gui::Button("Save")) {
			FILE* fp = fopen("data.bin", "wb");
			if (fp) {
				fwrite(&_num, sizeof(int), 1, fp);
				for (int i = 0; i < _num; ++i) {
					fwrite(&_items[i], sizeof(Sprite), 1, fp);
				}
				fclose(fp);
			}
		}
		if (gui::Button("Clear")) {
			_num = 0;
		}
		gui::endGroup();
		gui::DropDownBox("Sprites", NAMES, _num, &_spriteSelectionState, &_selection, &_spriteSelectionOffset, 6);
		if (gui::Button("Add")) {
			++_num;
		}		
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
	gui::end();

	_sprites->begin();
	/*
	ds::Color clr(255, 255, 255, 255);
	for (int i = 0; i < 5; ++i) {
		clr.r = 0.1f + i * 0.1f;
		clr.a = 0.5f + i * 0.1f;
		_sprites->add(ds::vec2(400+i*15, 384), ds::vec4(233, 74, 36, 36),ds::vec2(1,1),0.0f,clr);
	}
	*/
	for (int i = 0; i < _num; ++i) {
		const Sprite& s = _items[i];
		_sprites->add(ds::vec2(512, 384) + s.position, s.textureRect, s.scaling, s.rotation, s.color);
	}
	//_sprites->add(ds::vec2(500, 500), ds::vec4(270, 70, 48, 48), ds::vec2(1, 1), 0.0f, ds::Color(255,0,0,255));
	//_sprites->add(ds::vec2(509, 500), ds::vec4(270, 70, 48, 48), ds::vec2(1, 1), 0.0f, ds::Color(0,255,0,255));

	_sprites->flush();
}