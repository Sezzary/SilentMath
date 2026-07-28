#pragma once

namespace Silent::Renderer
{
    /** @brief Per-frame GPU uniform data for post-process film grain data. */
    struct alignas(16) UniformFilmGrain
    {
        float Time = 0.0f;
    };
}
