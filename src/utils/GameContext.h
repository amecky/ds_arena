#pragma once
#include <stdint.h>
#include <diesel.h>
#include "..\particles\Particlesystem.h"
#include "..\lib\HexGrid.h"
#include "..\core\SpriteArray.h"
#include "background_grid.h"
// ---------------------------------------------------------------
// game settings
// ---------------------------------------------------------------
struct GameSettings {

	uint32_t maxSpawnEnemies;
	ds::Color playerHightlightColor;
	ds::Color wakeUpHightlightColor;
	ds::Color gridBaseColor;
	float prepareTTL;
	float prepareFlashingTTL;
	float fire_rate;
	float bullet_velocity;
	float player_velocity;
	float enemy_seek_velocity;
};

enum PSystems {
	PS_EXPLOSION,
	PS_DEATH,
	PS_LIGHT_STREAKS,
	PS_BULLET,
	PS_TRAIL,
	PS_WAKEUP
};

static const char* PS_NAMES[] = {
	"EXPLOSION",
	"DEATH",
	"LIGHT_STREAKS",
	"BULLET",
	"TRAIL",
	"WAKEUP"
};

// ---------------------------------------------------------------
// game context
// ---------------------------------------------------------------
struct GameContext {

	int score;
	int highscores[10];
	ParticleManager* particleManager;
	int enemyExplosion;
	int playerTrail;
	int wakeUpSystem;
	EmitterSettings emitterSettings[16];
	GameSettings settings;
	ds::vec2 world_size;
	ds::vec2 world_pos;
	HexGrid grid;
	SpriteArray sprites;
	SUID player_id;
	GridContext grid_context;
	
};
