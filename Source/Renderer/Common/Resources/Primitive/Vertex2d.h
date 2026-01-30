#pragma once

namespace Silent::Renderer
{
    /** @brief 2D screen vertex. */
    struct Vertex2d
    {
        Vector2 Position = Vector2::Zero;
        Color   Col      = Color::Clear;
        Vector2 Uv       = Vector2::Zero;
    };
}
