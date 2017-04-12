#pragma once
#include "GameContext.h"

namespace highscore {

	void load(const char* name, GameContext* ctx);

	int rank(GameContext* ctx, int value);

	void save(const char* name, GameContext* ctx);
}
