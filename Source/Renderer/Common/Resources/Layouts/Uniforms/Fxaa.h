#pragma once

namespace Silent::Renderer
{
    /** @brief Per-frame GPU uniform data for FXAA post-process. */
    struct alignas(16) UniformFxaa
    {
        Vector2 Resolution    = Vector2::Zero;
        float   VirtualHeight = 0.0f;
    };
}
