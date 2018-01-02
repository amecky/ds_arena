#include "Bullets.h"
#include "..\lib\math.h"

// ---------------------------------------------------------------
// objects / general
// ---------------------------------------------------------------
namespace objects {

	ds::vec2 convert_mouse_to_world(GameContext* ctx) {
		ds::vec2 mp = ds::getMousePosition();
		float dx = (ctx->world_pos.x - 640.0f) / 256.0f;
		mp.x += dx * 256.0f;
		float dy = (ctx->world_pos.y - 360.0f) / 144.0f;
		mp.y += dy * 144.0f;
		return mp;
	}

	void preapre_updates(SpriteArray& sprites) {
		ds::vec2* forces = sprites.forces;
		for (uint32_t i = 0; i < sprites.num; ++i) {
			*forces = ds::vec2(0.0f);
			++forces;
		}
	}

	void apply_forces(SpriteArray& sprites, float dt) {
		ds::vec2* forces = sprites.forces;
		ds::vec2* positions = sprites.positions;
		for (uint32_t i = 0; i < sprites.num; ++i) {
			*positions += *forces * dt;
			++forces;
			++positions;
		}
	}

	SUID get_selected_object(GameContext* ctx, const ds::vec2& pos, float radius) {
		float dist = 0.0f;
		ds::vec2 pnv;
		ds::vec2* positions = ctx->sprites.positions;
		SUID* suids = ctx->sprites.ids;
		for (uint32_t i = 0; i < ctx->sprites.num; ++i) {
			if (math::checkCircleIntersection(pos, radius, *positions, 20.0f, &dist, &pnv)) {
				return *suids;
			}
			++positions;
			++suids;
		}
		return INVALID_SUID;
	}
}

// ---------------------------------------------------------------
// bullets
// ---------------------------------------------------------------
namespace bullets {

	// ---------------------------------------------------------------
	// move bullets
	// ---------------------------------------------------------------
	int move(SpriteArray& sprites, float dt, BackgroundState* background_state, const ds::vec2& world_size, ds::vec2* killed_positions, int max) {
		ds::vec2* forces = sprites.forces;
		ds::vec2* velocities = sprites.velocities;
		SpriteType::Enum* types = sprites.types;
		SUID* suids = sprites.ids;
		int counter = 0;
		for (uint32_t i = 0; i < sprites.num; ++i) {
			if (*types == SpriteType::BULLET) {
				*forces = *velocities;
			}
			++forces;
			++types;
			++velocities;
			++suids;
		}

		ds::vec2* positions = sprites.positions;
		forces = sprites.forces;
		suids = sprites.ids;
		types = sprites.types;
		uint32_t cnt = 0;
		while (cnt < sprites.num) {
			bool cont = true;
			if (*types == SpriteType::BULLET) {
				ds::vec2 tmp = *positions + *forces * dt;
				if (tmp.x < 55.0f && tmp.x >(world_size.x - 50.0f) && tmp.y < 55.0f && tmp.y >(world_size.y - 50.0f)) {
					if (counter < max) {
						killed_positions[counter++] = tmp;
					}
					sprites::remove(sprites, *suids);
					cont = false;
				}
				else if (background_state->borderCollision(tmp, 4.0f)) {
					if (counter < max) {
						killed_positions[counter++] = tmp;
					}
					sprites::remove(sprites, *suids);
					cont = false;
				}
			}
			if (cont) {
				++cnt;
				++forces;
				++positions;
				++suids;
				++types;
			}
		}

		return counter;
	}

	// ---------------------------------------------------------------
	// add bullet
	// ---------------------------------------------------------------
	void add(GameContext* ctx, float radius_offset, float angle_offset) {
		uint32_t idx = sprites::get_index(ctx->sprites, ctx->player_id);
		ds::vec2 bp = ctx->sprites.positions[idx];
		float angle = ctx->sprites.rotations[idx];
		if (angle_offset != 0.0f) {
			bp += ds::vec2(cos(angle + ds::PI * 0.5f) * angle_offset, sin(angle + ds::PI * 0.5f) * angle_offset);
			bp += ds::vec2(cos(angle) * radius_offset, sin(angle) * radius_offset);
		}
		SUID bid = sprites::create(ctx->sprites, bp, ds::vec4(120, 60, 8, 8), SpriteType::BULLET, 0, angle, 3.0f, 0.5f, ds::Color(42, 202, 236, 255));
		uint32_t bidx = sprites::get_index(ctx->sprites, bid);
		ctx->sprites.velocities[bidx] = ds::vec2(cos(angle), sin(angle)) * ctx->settings.bullet_velocity;

	}

}

// ---------------------------------------------------------------
// player
// ---------------------------------------------------------------
namespace player {

	// ---------------------------------------------------------------
	// create player
	// ---------------------------------------------------------------
	void create(GameContext* ctx, const ds::vec2& pos) {
		ctx->player_id = sprites::create(ctx->sprites,pos, ds::vec4(0, 40, 40, 40), SpriteType::PLAYER, 5, 0.0f,1.0f,1.0f,ds::Color(255, 0, 0, 255));
		ctx->world_pos = pos;
		uint32_t idx = sprites::get_index(ctx->sprites, ctx->player_id);
		ctx->sprites.previous[idx] = pos;
	}

	// ---------------------------------------------------------------
	// move player
	// ---------------------------------------------------------------
	void move(GameContext* ctx, float dt) {
		uint32_t idx = sprites::get_index(ctx->sprites, ctx->player_id);
		ds::vec2 player_pos = ctx->sprites.positions[idx];
		ds::vec2 vel = ds::vec2(0.0f);
		if (ds::isKeyPressed('A')) {
			vel.x -= 1.0f;
		}
		if (ds::isKeyPressed('D')) {
			vel.x += 1.0f;
		}
		if (ds::isKeyPressed('W')) {
			vel.y += 1.0f;
		}
		if (ds::isKeyPressed('S')) {
			vel.y -= 1.0f;
		}
		ds::vec2 pos = player_pos + vel * ctx->settings.player_velocity * dt;

		if (pos.x > 55.0f && pos.x < (ctx->world_size.x - 50.0f) && pos.y > 55.0f && pos.y < (ctx->world_size.y - 50.0f)) {
			ctx->sprites.positions[idx] = pos;
			ds::vec2 previous = ctx->sprites.previous[idx];
			ds::vec2 d = pos - previous;
			if (sqr_length(d) > 100.0f) {
				ctx->particleManager->emitt(ctx->playerTrail, previous, ctx->emitterSettings[PSystems::PS_TRAIL]);
				ctx->sprites.previous[idx] = pos;
			}
		}
		ds::vec2 mp = objects::convert_mouse_to_world(ctx);
		ctx->sprites.rotations[idx] = math::getAngle(pos, mp);
	}

	// ---------------------------------------------------------------
	// move camera
	// ---------------------------------------------------------------
	void move_camera(GameContext* ctx, float dt) {
		uint32_t idx = sprites::get_index(ctx->sprites, ctx->player_id);
		ds::vec2 player_pos = ctx->sprites.positions[idx];
		ds::vec2 diff = player_pos - ctx->world_pos;
		if (sqr_length(diff) > 16.0f) {
			ds::vec2 n = normalize(diff);
			ds::vec2 desired = n * 350.0f;
			ctx->world_pos += desired * dt;
		}
		//_backgroundState->highlight(_player.pos, ctx->settings.playerHightlightColor);

		float mx = ctx->world_size.x - 640.0f;
		float my = ctx->world_size.y - 360.0f;

		if (ctx->world_pos.x < 640.0f) {
			ctx->world_pos.x = 640.0f;
		}
		if (ctx->world_pos.x > mx) {
			ctx->world_pos.x = mx;
		}
		if (ctx->world_pos.y < 360.0f) {
			ctx->world_pos.y = 360.0f;
		}
		if (ctx->world_pos.y > my) {
			ctx->world_pos.y = my;
		}
		//_backgroundState->setScreenCenter(ctx->world_pos);
	}

	ds::vec2 get_position(GameContext* ctx) {
		uint32_t idx = sprites::get_index(ctx->sprites, ctx->player_id);
		return ctx->sprites.positions[idx];
	}
}

// ---------------------------------------------------------------
// enemies
// ---------------------------------------------------------------
namespace enemies {

	const static ds::vec4 ENEMY_TEXTURES[] = {
		ds::vec4(120,0,32,32),
		ds::vec4(160,0,32,32),
	};

	// ---------------------------------------------------------------
	// create
	// ---------------------------------------------------------------
	SUID create(GameContext* ctx, const ds::vec2& pos, int type) {
		SUID id = sprites::create(ctx->sprites, pos, ENEMY_TEXTURES[type], SpriteType::ENEMY);
		return id;
	}

	// ---------------------------------------------------------------
	// seek player
	// ---------------------------------------------------------------
	void seek_player(GameContext* ctx) {
		ds::vec2 player_pos = player::get_position(ctx);
		ds::vec2* positions = ctx->sprites.positions;
		ds::vec2* forces = ctx->sprites.forces;
		SpriteType::Enum* types = ctx->sprites.types;
		for (uint32_t i = 0; i < ctx->sprites.num; ++i) {
			if (*types == SpriteType::ENEMY) {
				ds::vec2 diff = player_pos - *positions;
				ds::vec2 n = normalize(diff);
				*forces = n * ctx->settings.enemy_seek_velocity;
			}
			++positions;
			++types;
			++forces;
		}
	}

	void separate(GameContext* ctx) {
		const float minDistance = 30.0f;
		const float relaxation = 10.0f;
		float sqrDist = minDistance * minDistance;
		SpriteType::Enum* types = ctx->sprites.types;
		ds::vec2* positions = ctx->sprites.positions;
		ds::vec2* forces = ctx->sprites.forces;
		for (uint32_t i = 0; i < ctx->sprites.num; ++i) {
			if (*types == SpriteType::ENEMY) {
				ds::vec2 currentPos = *positions;
				SpriteType::Enum* other_types = ctx->sprites.types;
				ds::vec2* other_positions = ctx->sprites.positions;
				for (uint32_t j = 0; j < ctx->sprites.num; ++j) {
					if (*other_types == SpriteType::ENEMY && i != j) {						
						ds::vec2 dist = *other_positions - currentPos;
						if (sqr_length(dist) < sqrDist) {
							ds::vec2 separationForce = dist;
							separationForce = normalize(separationForce);
							separationForce = separationForce * relaxation;
							*forces -= separationForce;
						}
					}
					++other_types;
					++other_positions;
				}
			}
			++types;
			++positions;
			++forces;
		}
	}

	// ---------------------------------------------------------------
	// look at player
	// ---------------------------------------------------------------
	void look_at_player(GameContext* ctx) {
		ds::vec2 player_pos = player::get_position(ctx);
		ds::vec2* positions = ctx->sprites.positions;
		float* rotations = ctx->sprites.rotations;
		SpriteType::Enum* types = ctx->sprites.types;
		int counter = 0;
		for (uint32_t i = 0; i < ctx->sprites.num; ++i) {
			if (*types == SpriteType::ENEMY) {
				*rotations = math::getAngle(*positions, player_pos);
			}
			++positions;
			++types;
			++rotations;
		}

	}

	// ---------------------------------------------------------------
	// get first enemy
	// ---------------------------------------------------------------
	SUID get_first(GameContext* ctx) {
		SpriteType::Enum* types = ctx->sprites.types;
		SUID* suids = ctx->sprites.ids;
		for (uint32_t i = 0; i < ctx->sprites.num; ++i) {
			if (*types == SpriteType::ENEMY) {
				return *suids;
			}
			++types;
			++suids;
		}
		return INVALID_SUID;
	}

	// ---------------------------------------------------------------
	// check enemy collisions
	// ---------------------------------------------------------------
	SUID check_collision(GameContext* ctx, const ds::vec2& pos, float radius) {
		float dist = 0.0f;
		ds::vec2 pnv;
		ds::vec2* positions = ctx->sprites.positions;
		SpriteType::Enum* types = ctx->sprites.types;
		SUID* suids = ctx->sprites.ids;
		int counter = 0;
		for (uint32_t i = 0; i < ctx->sprites.num; ++i) {
			if (*types == SpriteType::ENEMY) {
				if (math::checkCircleIntersection(pos, radius, *positions, 20.0f, &dist, &pnv)) {
					return *suids;
				}
			}
			++positions;
			++types;
			++suids;
		}
		return INVALID_SUID;
	}

}