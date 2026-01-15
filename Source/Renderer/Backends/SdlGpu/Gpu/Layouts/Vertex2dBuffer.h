#pragma once

namespace Silent::Renderer
{
    /** @brief 2D GPU buffer vertex. */
    struct Vertex2dBuffer
    {
        Vector3 Position = Vector3::Zero;
        Vector2 Uv       = Vector2::Zero;
        Color   Col      = Color::Clear;
    };
}
