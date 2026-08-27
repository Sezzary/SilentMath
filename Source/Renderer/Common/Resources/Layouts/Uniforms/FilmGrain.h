#pragma once

namespace Silent::Renderer
{
    /** @brief Per-frame GPU uniform data for post-process film grain. */
    struct alignas(16) UniformFilmGrainPerFrame
    {
        float Time = 0.0f;
    };
}
