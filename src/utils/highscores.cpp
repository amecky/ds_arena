#include "highscores.h"
#include <stdio.h>

namespace highscore {

	void load(const char* name, GameContext* ctx) {
		FILE* fp = fopen(name, "rb");
		if (fp) {
			for (int i = 0; i < 10; ++i) {
				fread(&ctx->highscores[i], sizeof(int), 1, fp);
			}
			fclose(fp);
		}
		else {
			for (int i = 0; i < 10; ++i) {
				ctx->highscores[i] = 0;
			}
			save(name, ctx);
		}
	}

	int rank(GameContext* ctx, int value) {
		int m = -1;
		for (int i = 0; i < 10; ++i) {
			if (m == -1 && value >= ctx->highscores[i]) {
				m = i;
			}
		}
		if (m != -1) {
			for (int i = 8; i > m; --i) {
				ctx->highscores[i + 1] = ctx->highscores[i];
			}
			ctx->highscores[m] = value;
		}
		return m;
	}

	void save(const char* name, GameContext* ctx) {
		FILE* fp = fopen(name, "wb");
		if (fp) {
			for (int i = 0; i < 10; ++i) {
				fwrite(&ctx->highscores[i], sizeof(int), 1, fp);
			}
			fclose(fp);
		}
	}
}
