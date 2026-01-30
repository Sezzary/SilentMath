#pragma once

namespace Silent::Renderer
{
    /** @brief GPU uniform buffer for post-process CRT shader. */
    struct alignas(16) UniformCrt
    {
        Vector2 Resolution = 0.0f;
        float   Time       = 0.0f;
    };
}
