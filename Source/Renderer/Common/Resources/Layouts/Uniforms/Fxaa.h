#pragma once

namespace Silent::Renderer
{
    /** @brief Per-frame GPU uniform data for FXAA post-process. */
    struct alignas(16) UniformFxaaPerFrame
    {
        Vector2 Resolution = Vector2::Zero;
    };
}
