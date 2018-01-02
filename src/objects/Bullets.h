#pragma once
#include <diesel.h>
#include "..\utils\GameContext.h"
#include "..\states\GameState.h"

namespace objects {

	ds::vec2 convert_mouse_to_world(GameContext* ctx);

	void preapre_updates(SpriteArray& sprites);

	void apply_forces(SpriteArray& sprites, float dt);

	SUID get_selected_object(GameContext* ctx, const ds::vec2& pos, float radius);

}

namespace bullets {


	int move(SpriteArray& sprites, float dt, BackgroundState* background_state, const ds::vec2& world_size, ds::vec2* killed_positions, int max);

	void add(GameContext* ctx, float radius_offset, float angle_offset);

}

namespace player {

	void create(GameContext* ctx, const ds::vec2& pos);

	void move(GameContext* ctx, float dt);

	ds::vec2 get_position(GameContext* ctx);

	void move_camera(GameContext* ctx, float dt);
}

namespace enemies {

	SUID get_first(GameContext* ctx);

	SUID create(GameContext* ctx, const ds::vec2& pos, int type);

	void look_at_player(GameContext* ctx);

	void seek_player(GameContext* ctx);

	void separate(GameContext* ctx);

	SUID check_collision(GameContext* ctx, const ds::vec2& pos, float radius);

}