#pragma once
#include <stdint.h>
#include <diesel.h>
#include "..\particles\Particlesystem.h"
// ---------------------------------------------------------------
// game settings
// ---------------------------------------------------------------
struct GameSettings {

	int maxSpawnEnemies;
	ds::Color playerHightlightColor;
	ds::Color wakeUpHightlightColor;
	ds::Color gridBaseColor;
	float prepareTTL;
	float prepareFlashingTTL;
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
	EmitterSettings explosionSettings;
	EmitterSettings deathSettings;
	EmitterSettings bulletExplosionSettings;
	EmitterSettings playerTrailSettings;
	EmitterSettings wakeupSettings;

	GameSettings settings;

};
