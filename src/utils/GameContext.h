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

};

// ---------------------------------------------------------------
// game context
// ---------------------------------------------------------------
struct GameContext {

	int score;
	int highscores[10];

	GameSettings settings;

};
