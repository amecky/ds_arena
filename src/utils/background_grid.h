#pragma once
#include <diesel.h>

struct GridPoint {
	ds::vec2 position;
	ds::vec2 old_pos;
	ds::vec2 velocity;
	float invMass;
	float damping;
	ds::vec2 acceleration;
	bool movable;

	float timer;
	bool marked;
	ds::Color color;
};

struct WarpingGridVertex {
	ds::vec3 pos;
	ds::vec2 uv;
	ds::Color color;
};

struct WarpingGridSettings {
	int numX;
	int numY;
	float stiffness;// = 0.28f;
	float damping;// = 0.06f;
	ds::vec4 texture_rect;
};

struct Spring {
	int sx;
	int sy;
	int ex;
	int ey;
	float targetLength;
	float stiffness;
	float damping;

};

struct GridBuffer {
	ds::vec4 screen_center;
	ds::matrix wvp;
};

struct GridContext {
	int size_x;
	int size_y;
	GridPoint* points;
	uint16_t* indices;
	WarpingGridVertex* vertices;
	int num_vertices;
	Spring* springs;
	int num_springs;
	GridBuffer buffer;
	RID draw_item;
	RID vertex_buffer;
	RID render_pass;
	float timer;
	ds::vec2 screen_center;
};

namespace grid {

	void create(GridContext* ctx, const WarpingGridSettings& settings);

	void render(GridContext* ctx);

	void apply_force(GridContext* ctx, int x, int y, const ds::vec2& force);

	void apply_force(GridContext* ctx, const ds::vec2& center, float radius, float force);

	void tick(GridContext* ctx, float dt);
}