#pragma once

namespace Silent::Renderer
{
    /** @brief GPU buffer for 2D vertex. */
    struct BufferVertex2d
    {
        Vector3 Position = Vector3::Zero;
        Vector2 Uv       = Vector2::Zero;
        Color   Col      = Color::Clear;
    };
}
