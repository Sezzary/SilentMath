#pragma once

namespace Silent::Renderer
{
    /** @brief GPU buffer for 3D vertex. */
    struct BufferVertex3d
    {
        Vector3 Position = Vector3::Zero;
        Vector3 Normal   = Vector3::One;
        Vector2 Uv       = Vector2::Zero;
        Color   Col      = Color::Clear;
    };
}
