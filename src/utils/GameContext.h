#pragma once
#include <stdint.h>
#include "..\..\..\diesel\diesel.h"
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

	GameSettings settings;

};
