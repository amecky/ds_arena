#pragma once
#include "..\..\diesel\diesel.h"

namespace sprites {

	void init(int maxSprites, RID textureID);

	void begin();

	void add(const ds::vec2& position, const ds::vec4& rect, const ds::vec2& scale = ds::vec2(1, 1), float rotation = 0.0f, const ds::Color& clr = ds::Color(255, 255, 255, 255));

	void flush();

	void shutdown();
}