#pragma once

namespace Silent::Renderer
{
    /** @brief Per-frame GPU uniform data for 3D scene pixelization effect data. */
    struct alignas(16) UniformPixelize
    {
        Vector2 Resolution    = Vector2::Zero;
        float   VirtualHeight = 0.0f;
    };
}
