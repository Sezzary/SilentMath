#pragma once

namespace Silent::Renderer
{
    struct Sprite
    {
        static constexpr uint UV_COUNT = 4;

		int      TextureIdx = 0;
        Vector2i Resolution = Vector2i::Zero;
		Vector2  Uvs[UV_COUNT];
    };
}
