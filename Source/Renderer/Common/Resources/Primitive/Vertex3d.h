#pragma once

namespace Silent::Renderer
{
    /** @brief 3D world vertex. */
    struct Vertex3d
    {
        Vector3 Position = Vector3::Zero;
        Vector3 Normal   = Vector3::One;
        Vector2 Uv       = Vector2::Zero;
        Color   Col      = Color::Clear;
    };
}
