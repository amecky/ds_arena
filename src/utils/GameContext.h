#pragma once
#include <stdint.h>
#include <diesel.h>
#include "..\particles\Particlesystem.h"
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

};
