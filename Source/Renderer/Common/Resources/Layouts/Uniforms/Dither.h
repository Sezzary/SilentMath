#pragma once

namespace Silent::Renderer
{
    /** @brief Per-frame GPU uniform data for 3D scene dithering post-process. */
    struct alignas(16) UniformDitherPerFrame
    {
        Vector2 Resolution    = Vector2::Zero;
        float   VirtualHeight = 0.0f;
    };
}
