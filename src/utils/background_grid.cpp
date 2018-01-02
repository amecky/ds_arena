#include "background_grid.h"
#include "BackgroundGrid_VS_Main.h"
#include "BackgroundGrid_PS_Main.h"

namespace grid {

	ds::vec2 convert_grid_coords(int x, int y) {
		return ds::vec2(20.0f + x * 40.0f, 20.0f + y * 40.0f);
	}
	// ----------------------------------------------------
	// add spring
	// ----------------------------------------------------
	static void add_spring(GridContext* ctx, int x1, int y1, int x2, int y2, float stiffness, float damping) {
		Spring& spring = ctx->springs[ctx->num_springs++];
		spring.sx = x1;
		spring.sy = y1;
		spring.ex = x2;
		spring.ey = y2;
		spring.stiffness = stiffness;
		spring.damping = damping;
		const GridPoint& sp = ctx->points[x1 + y1 * ctx->size_x];
		const GridPoint& ep = ctx->points[x2 + y2 * ctx->size_x];
		spring.targetLength = distance(sp.position, ep.position);// *0.95f;
	}

	void create(GridContext* ctx, const WarpingGridSettings& settings) {
		ctx->num_springs = 0;
		ctx->size_x = settings.numX;
		ctx->size_y = settings.numY;
		ctx->timer = 0.0f;
		ctx->points = new GridPoint[settings.numX * settings.numY];
		ctx->springs = new Spring[settings.numX * settings.numY * 2];
		for (int y = 0; y < settings.numY; ++y) {
			for (int x = 0; x < settings.numX; ++x) {
				GridPoint& vp = ctx->points[x + y * settings.numX];
				vp.position = convert_grid_coords(x, y);
				vp.acceleration = ds::vec2(0.0f);
				vp.damping = 0.98f;
				vp.invMass = 1.0f;
				vp.movable = true;
				vp.color = ds::Color(32,32,32,255);
				vp.timer = 0.0f;
				vp.marked = false;
				if (x == 0 || y == 0 || x == (settings.numX - 1) || y == (settings.numY - 1)) {
					vp.invMass = 0.0f;
					vp.movable = false;
				}
				vp.velocity = ds::vec2(0.0f);
				vp.old_pos = vp.position;
			}
		}

		for (int y = 0; y < settings.numY; ++y) {
			for (int x = 0; x < settings.numX; ++x) {
				if (x > 0)
					add_spring(ctx, x - 1, y, x, y, settings.stiffness, settings.damping);
				if (y > 0)
					add_spring(ctx, x, y - 1, x, y, settings.stiffness, settings.damping);
			}
		}

		int cnt = 0;
		ctx->num_vertices = (settings.numX - 1) * (settings.numY - 1) * 6;
		ctx->indices = new uint16_t[ctx->num_vertices];
		for (int y = 0; y < settings.numY - 1; ++y) {
			for (int x = 0; x < settings.numX - 1; ++x) {
				int idx = x + y * settings.numX;
				ctx->indices[cnt++] = idx + settings.numX;
				ctx->indices[cnt++] = idx + 1 + settings.numX;
				ctx->indices[cnt++] = idx;
				ctx->indices[cnt++] = idx + 1 + settings.numX;
				ctx->indices[cnt++] = idx + 1;
				ctx->indices[cnt++] = idx;
			}
		}

		ctx->vertices = new WarpingGridVertex[ctx->num_vertices];
		float u1 = 350.0f / 1024.0f;
		float u2 = u1 + 40.0f / 1024.0f;
		float v1 = 70.0f / 1024.0f;
		float v2 = v1 + 40.0f / 1024.0f;

		const ds::vec2 UVS[] = { ds::vec2(u1,v1),ds::vec2(u2,v1),ds::vec2(u1,v2),ds::vec2(u2,v1),ds::vec2(u2,v2),ds::vec2(u1,v2) };
		for (int i = 0; i < ctx->num_vertices; ++i) {
			ctx->vertices[i].pos = ctx->points[ctx->indices[i]].position;
			ctx->vertices[i].uv = UVS[i % 6];
			ctx->vertices[i].color = ds::Color(32,32,32,255);
		}

		ds::BlendStateInfo blendInfo = { ds::BlendStates::SRC_ALPHA, ds::BlendStates::SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, true };
		RID bs_id = ds::createBlendState(blendInfo);

		RID bumpVS = ds::createVertexShader(BackgroundGrid_VS_Main, sizeof(BackgroundGrid_VS_Main));
		RID bumpPS = ds::createPixelShader(BackgroundGrid_PS_Main, sizeof(BackgroundGrid_PS_Main));

		ds::matrix orthoView = ds::matIdentity();
		ds::matrix orthoProjection = ds::matOrthoLH(ds::getScreenWidth(), ds::getScreenHeight(), 0.0f, 1.0f);
		ds::Camera camera = {
			orthoView,
			orthoProjection,
			orthoView * orthoProjection,
			ds::vec3(0,0,0),
			ds::vec3(0,0,1),
			ds::vec3(0,1,0),
			ds::vec3(1,0,0),
			0.0f,
			0.0f,
			0.0f
		};
		ds::ViewportInfo vpInfo = { ds::getScreenWidth(), ds::getScreenHeight(), 0.0f, 1.0f };
		RID vp = ds::createViewport(vpInfo, "SpriteOrthoViewport");
		ds::RenderPassInfo rpInfo = { &camera, vp, ds::DepthBufferState::DISABLED, 0, 0 };
		ctx->render_pass = ds::createRenderPass(rpInfo, "SpritesOrthoPass");

		ds::InputLayoutDefinition decl[] = {
			{ "POSITION" , 0, ds::BufferAttributeType::FLOAT3 },
			{ "TEXCOORD" , 0, ds::BufferAttributeType::FLOAT2 },
			{ "COLOR"    , 0, ds::BufferAttributeType::FLOAT4 }
		};
		ds::InputLayoutInfo ilInfo = { decl, 3, bumpVS };
		RID rid = ds::createInputLayout(ilInfo);

		ctx->buffer.wvp = ds::matTranspose(camera.viewProjectionMatrix);
		ctx->buffer.screen_center = ds::vec4(640, 360,0,0);

		RID cbid = ds::createConstantBuffer(sizeof(GridBuffer), &ctx->buffer);

		ds::VertexBufferInfo vbInfo = { ds::BufferType::DYNAMIC,ctx->num_vertices,sizeof(WarpingGridVertex),ctx->vertices };
		ctx->vertex_buffer = ds::createVertexBuffer(vbInfo);

		ds::SamplerStateInfo samplerInfo = { ds::TextureAddressModes::CLAMP, ds::TextureFilters::LINEAR };
		RID ssid = ds::createSamplerState(samplerInfo);

		RID textureID = ds::findResource(SID("content\\TextureArray.png"), ds::ResourceType::RT_SRV);

		RID stateGroup = ds::StateGroupBuilder()
			.inputLayout(rid)
			.constantBuffer(cbid, bumpVS, 0)
			.blendState(bs_id)
			.texture(textureID,bumpPS,0)
			.samplerState(ssid, bumpPS)
			.vertexBuffer(ctx->vertex_buffer)
			.vertexShader(bumpVS)
			.pixelShader(bumpPS)
			.build();

		ds::DrawCommand drawCmd = { ctx->num_vertices, ds::DrawType::DT_VERTICES, ds::PrimitiveTypes::TRIANGLE_LIST };

		ctx->draw_item = ds::compile(drawCmd, stateGroup);

	}

	// ----------------------------------------------------
	// render
	// ----------------------------------------------------
	void render(GridContext* ctx) {
		ctx->buffer.screen_center = ds::vec4(ctx->screen_center.x, ctx->screen_center.y, 0.0f, 0.0f);
		for (int i = 0; i < ctx->num_vertices; ++i) {
			ctx->vertices[i].pos = ctx->points[ctx->indices[i]].position;
			ctx->vertices[i].color = ctx->points[ctx->indices[i]].color;
		}
		ds::mapBufferData(ctx->vertex_buffer, ctx->vertices, sizeof(WarpingGridVertex) * ctx->num_vertices);
		ds::submit(ctx->render_pass, ctx->draw_item);
	}

	const float TIME_STEPSIZE2 = 0.95f * 0.95f;

	// ----------------------------------------------------
	// apply force
	// ----------------------------------------------------
	void apply_force(GridContext* ctx, int x, int y, const ds::vec2& force) {
		GridPoint& gp = ctx->points[x + y * ctx->size_x];
		gp.acceleration += force * gp.invMass;
	}

	void apply_force(GridContext* ctx, const ds::vec2& center, float radius, float force) {
		for (int y = 0; y < ctx->size_y; ++y) {
			for (int x = 0; x < ctx->size_x; ++x) {
				GridPoint& gp = ctx->points[x + y * ctx->size_x];
				ds::vec2 delta = gp.position - center;
				if (sqr_length(delta) < radius * radius) {
					gp.acceleration += normalize(delta) * force * gp.invMass * -1.0f;
				}
			}
		}
	}
	// ----------------------------------------------------
	// tick
	// ----------------------------------------------------
	void tick(GridContext* ctx, float dt) {
		float timeStep = 1.0f / 60.0f;
		ctx->timer += ds::getElapsedSeconds();
		if (ctx->timer > 0.25f) {
			ctx->timer = 0.25f;
		}
		int cnt = 0;
		while (ctx->timer >= timeStep) {
			for (int y = 0; y < ctx->size_y; ++y) {
				for (int x = 0; x < ctx->size_x; ++x) {
					GridPoint& gp = ctx->points[x + y * ctx->size_x];
					if (gp.movable) {
						ds::vec2 temp = gp.position;
						gp.position = gp.position + (gp.position - gp.old_pos) * gp.damping + gp.acceleration * TIME_STEPSIZE2;
						gp.old_pos = temp;
						gp.acceleration = ds::vec2(0.0f);
					}					
				}
			}

			for (uint32_t i = 0; i < ctx->num_springs; ++i) {
				Spring& spring = ctx->springs[i];
				GridPoint& sp = ctx->points[spring.sx + spring.sy * ctx->size_x];
				GridPoint& ep = ctx->points[spring.ex + spring.ey * ctx->size_x];
				ds::vec2 delta = ep.position - sp.position;
				float current_distance = length(delta);
				ds::vec2 x = sp.position - ep.position;
				float l = length(x);
				if (l > spring.targetLength) {
					x = (x / l) * (l - spring.targetLength);
					ds::vec2 dv = ep.velocity - sp.velocity;
					ds::vec2 force = spring.stiffness * x - dv * spring.damping;
					sp.acceleration -= force;
					ep.acceleration += force;
				}
			}
			ctx->timer -= timeStep;
			++cnt;
		}
	}

}