#pragma once

namespace Silent::Renderer
{
    /** @brief Per-frame GPU uniform data for CRT post-process. */
    struct alignas(16) UniformCrtPerFrame
    {
        Vector2 Resolution = 0.0f;
        float   Time       = 0.0f;
    };
}
