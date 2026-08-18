#pragma once

namespace Silent::Renderer
{
    /** @brief Per-frame GPU uniform data for 3D scene dithering effect data. */
    struct alignas(16) UniformDither
    {
        Vector2 Resolution    = Vector2::Zero;
        float   VirtualHeight = 0.0f;
    };
}
