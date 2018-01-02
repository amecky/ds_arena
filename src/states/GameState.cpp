#include "GameState.h"
#include "..\utils\hud.h"
#include "..\utils\highscores.h"
#include <ds_tweakable.h>
#define DS_IMGUI_IMPLEMENTATION
#include <ds_imgui.h>
// -------------------------------------------------------
// check if mouse cursor is inside box
// -------------------------------------------------------
static bool isCursorInside(const ds::vec2& p, const ds::vec2& dim) {
	ds::vec2 mp = ds::getMousePosition();
	if (mp.x < (p.x - dim.x * 0.5f)) {
		return false;
	}
	if (mp.x >(p.x + dim.x * 0.5f)) {
		return false;
	}
	if (mp.y < (p.y - dim.y * 0.5f)) {
		return false;
	}
	if (mp.y >(p.y + dim.y * 0.5f)) {
		return false;
	}
	return true;
}

// ---------------------------------------------------------------
// Prepare state
// ---------------------------------------------------------------
int PrepareState::tick(float dt, EventStream* stream) {
	_prepareTimer += dt;
	if (_prepareTimer >= _ctx->settings.prepareTTL) {
		stream->add(ET_PREPARE_ELAPSED);
	}
	return 0;
}

void PrepareState::render(SpriteBatchBuffer* buffer) {
	float s = 1.0f;
	if (_prepareTimer <= _ctx->settings.prepareFlashingTTL) {
		float norm = _prepareTimer / _ctx->settings.prepareFlashingTTL;
		s = _scalePath.get(norm);
	}
	buffer->add(ds::vec2(512, 384), ds::vec4(544, 274, 380, 70),ds::vec2(s,s),0.0f,ds::Color(17,236,228,255));
}

void PrepareState::activate() {
	_prepareTimer = 0.0f;
	_active = true;
}

void PrepareState::deactivate() {
	_prepareTimer = 0.0f;
	_active = false;
}

// ---------------------------------------------------------------
// Game over state
// ---------------------------------------------------------------
void GameOverState::activate() {
	_active = true;
	_rank = highscore::rank(_ctx, _ctx->score);
}

int GameOverState::tick(float dt, EventStream* stream) {
	if (ds::isMouseButtonClicked(0)) {
		if (isCursorInside(ds::vec2(512, 220), ds::vec2(300, 56))) {
			stream->add(ET_GAME_OVER_PLAY);
			return 1;
		}
		if (isCursorInside(ds::vec2(512, 120), ds::vec2(300, 56))) {
			stream->add(ET_GAME_OVER_EXIT);
			return 2;
		}
	}
	return 0;
}

void GameOverState::render(SpriteBatchBuffer* buffer) {
	buffer->add(ds::vec2(512, 580), ds::vec4(532, 28, 400, 70),ds::vec2(1.0f),0.0f,ds::Color(255,0,0,255));
	buffer->add(ds::vec2(512, 220), ds::vec4(0, 360, 300, 56));
	buffer->add(ds::vec2(512, 120), ds::vec4(0, 300, 300, 56));
	// score
	buffer->add(ds::vec2(512, 470), ds::vec4(80, 120, 124, 40));
	numbers::draw(buffer,ds::vec2(440, 420), _ctx->score, 6);
	// rank
	buffer->add(ds::vec2(512, 350), ds::vec4(212, 120, 110, 40));
	if (_rank >= 0) {
		numbers::draw(buffer,ds::vec2(500, 300), _rank + 1, 2);
	}
}

// ---------------------------------------------------------------
// highscore state
// ---------------------------------------------------------------
int HighscoreState::tick(float dt, EventStream* stream) {
	_timer += dt;
	if (_timer >= 2.0f) {
		_timer -= 2.0f;
		_index += 5;
		_index = _index & 5;
	}
	if (ds::isMouseButtonClicked(0)) {
		if (isCursorInside(ds::vec2(512, 120), ds::vec2(300, 56))) {
			stream->add(ET_HIGHSCORES_EXIT);
			return 2;
		}
	}
	return 0;
}

void HighscoreState::render(SpriteBatchBuffer* buffer) {
	buffer->add(ds::vec2(512, 580), ds::vec4(518, 185, 420, 84),ds::vec2(1.0f),0.0f,ds::Color(182,255,134,255));
	buffer->add(ds::vec2(512, 120), ds::vec4(0, 300, 300, 56));
	int cnt = 0;
	float norm = _timer / 2.0f;
	float a = 1.0f;
	if (norm < 0.25f) {
		a = norm / 0.25f;
	}
	if (norm > 0.75f) {
		a = 1.0f - norm / 0.75f;
	}
	for (int i = _index; i < (_index + 5); ++i) {
		numbers::draw(buffer, ds::vec2(380, 500 - cnt * 70), i + 1, 2, false, ds::Color(1.0f,1.0f,1.0f,a));
		// score
		numbers::draw(buffer, ds::vec2(470, 500 - cnt * 70), _ctx->highscores[i], 6, true, ds::Color(1.0f, 1.0f, 1.0f, a));
		++cnt;
	}
}

// ---------------------------------------------------------------
// MainMenuState
// ---------------------------------------------------------------
int MainMenuState::tick(float dt, EventStream* stream) {
	if (ds::isMouseButtonClicked(0)) {
		if (isCursorInside(ds::vec2(512, 400), ds::vec2(300, 56))) {
			stream->add(ET_MAIN_MENU_PLAY);
			return 1;
		}
		if (isCursorInside(ds::vec2(512, 200), ds::vec2(300, 56))) {
			stream->add(ET_MAIN_MENU_EXIT);
			return 2;
		}
		if (isCursorInside(ds::vec2(512, 300), ds::vec2(300, 56))) {
			stream->add(ET_MAIN_MENU_HIGHSCORES);
			return 2;
		}
	}
	return 0;
}

void MainMenuState::render(SpriteBatchBuffer* buffer) {
	buffer->add(ds::vec2(512, 580), ds::vec4(550, 104, 365, 84),ds::vec2(1.0f),0.0f,ds::Color(5,248,205,255));
	buffer->add(ds::vec2(512, 400), ds::vec4(0, 240, 300, 56));
	buffer->add(ds::vec2(512, 200), ds::vec4(0, 300, 300, 56));
	buffer->add(ds::vec2(512, 300), ds::vec4(0, 420, 300, 56));
}

// ---------------------------------------------------------------
// Background state
// ---------------------------------------------------------------
BackgroundState::BackgroundState(GameContext* ctx, const ElasticBorderSettings& borderSettings) : ArenaGameState(ctx, "BackgroundState") , _borderSettings(borderSettings) {
	_width = 50;
	_height = 40;
	_ctx->grid.resize(_width, _height);
	_ctx->grid.setBaseColor(ctx->settings.gridBaseColor);
	
	
	RID textureID = ds::findResource(SID("content\\TextureArray.png"), ds::ResourceType::RT_SRV);
	_borderSettings.textureID = textureID;
	_borders = new ElasticBorder(&_borderSettings);
	//_active = true;
}

BackgroundState::~BackgroundState() {
	delete _borders;
}

int BackgroundState::tick(float dt, EventStream* stream) {
	_ctx->grid.tick(dt);
	_borders->tick(dt);
	return 0;
}

void BackgroundState::highlight(const ds::vec2& p, const ds::Color& color) {
	_ctx->grid.highlight(p, color);
}

bool BackgroundState::borderCollision(const ds::vec2& p, float radius) {
	return _borders->collides(p, radius);
}

void BackgroundState::render(SpriteBatchBuffer* buffer) {
	// background
	/*
	for (int r = 0; r < _height; ++r) {
		int q_offset = r >> 1;
		int delta = _width - q_offset;
		for (int q = -q_offset; q < delta; ++q) {
			Hex h = Hex(q, r);
			if (_ctx->grid.isValid(h)) {
				GridItem& current = _ctx->grid.get(h);
				if (current.timer > 0.0f) {
					buffer->add(current.position, ds::vec4(320, 0, 42, 46), ds::vec2(1, 1), 0.0f, current.color);
				}
				else {
					buffer->add(current.position, ds::vec4(320, 0, 42, 46), ds::vec2(1, 1), 0.0f, current.color);
				}
			}
		}
	}
	buffer->flush();
	*/
	// elastic borders
	_borders->render();
}


// ---------------------------------------------------------------
// ParticlesTestState
// ---------------------------------------------------------------
int ParticlesTestState::tick(float dt, EventStream* stream) {
	_timer += dt;
	if (_timer >= 1.0f) {
		_timer = 0.0f;
		for (int i = 0; i < 6; ++i) {
			if (_flags[i]) {
				int px = ds::random(300, 900);
				int py = ds::random(100, 600);
				_ctx->particleManager->emitt(_ctx->enemyExplosion, ds::vec2(px, py), _ctx->emitterSettings[i]);
			}
		}
	}
	_ctx->particleManager->tick(dt);
	return 0;
}

void ParticlesTestState::render(SpriteBatchBuffer* buffer) {
	buffer->flush();
	_ctx->particleManager->render();
}

void ParticlesTestState::renderGUI() {
	gui::begin("Settings", &_dialogState);
	if (_dialogState == 1) {
		/*
		for (int i = 0; i < 5; ++i) {
			gui::Checkbox(PS_NAMES[i], &_flags[i]);
		}
		gui::DropDownBox("Particlesystem", PS_NAMES, 5, &_dropDownState, &_selectedSystem, &_dropDownOffset, 5, true);
		*/
		//gui::Button("Button");
		const char* cats[32];
		int num = twk_num_categories();
		for (int i = 0; i < num; ++i) {
			const char* n = twk_get_category_name(i);
			cats[i] = n;

		}
		gui::DropDownBox("Categories", cats, num, &_categoryState, &_selectedCategory, &_categoryOffset, 4, true);
	}
	if (_selectedCategory != -1) {
		Tweakable tweakables[32];
		int nt = twk_get_tweakables(_selectedCategory, tweakables, 32);
		for (int i = 0; i < nt; ++i) {
			const Tweakable& twk = tweakables[i];
			if (twk.type == TweakableType::ST_FLOAT) {
				gui::Input(twk.name, twk.ptr.fPtr);
			}
			else if (twk.type == TweakableType::ST_VEC2) {
				gui::Input(twk.name, twk.ptr.v2Ptr);
			}
			else if (twk.type == TweakableType::ST_VEC3) {
				gui::Input(twk.name, twk.ptr.v3Ptr);
			}
			else if (twk.type == TweakableType::ST_VEC4) {
				gui::Input(twk.name, twk.ptr.v4Ptr);
			}
		}
	}
	gui::Button("Save");
	/*
	if (_selectedSystem != -1) {
		gui::begin("Emitter", &_emitterState);
		gui::Input("Count", &_ctx->emitterSettings[_selectedSystem].count);
		gui::Input("TTL", &_ctx->emitterSettings[_selectedSystem].ttl);
		gui::Input("Angle var",&_ctx->emitterSettings[_selectedSystem].angleVariance);
		gui::Input("Radius",&_ctx->emitterSettings[_selectedSystem].radius);
		gui::Input("Radius var",&_ctx->emitterSettings[_selectedSystem].radiusVariance);
		gui::Input("Veclocity",&_ctx->emitterSettings[_selectedSystem].velocity);
		gui::Input("Vel var",&_ctx->emitterSettings[_selectedSystem].velocityVariance);
		gui::Input("Size", &_ctx->emitterSettings[_selectedSystem].size);
		gui::Input("Size var", &_ctx->emitterSettings[_selectedSystem].sizeVariance);
		gui::Input("Growth", &_ctx->emitterSettings[_selectedSystem].growth);
		gui::Input("Accel", &_ctx->emitterSettings[_selectedSystem].acceleration);
		gui::Input("Color", &_ctx->emitterSettings[_selectedSystem].color);
		gui::Input("Decay", &_ctx->emitterSettings[_selectedSystem].decay);
		gui::Input("Tex rect", &_ctx->emitterSettings[_selectedSystem].texRect);
	}
	*/
}