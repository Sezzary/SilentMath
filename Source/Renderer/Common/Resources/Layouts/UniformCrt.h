#pragma once

namespace Silent::Renderer
{
    /** @brief Per-frame GPU uniform data for post-process CRT data. */
    struct alignas(16) UniformCrt
    {
        Vector2 Resolution = 0.0f;
        float   Time       = 0.0f;
    };
}
