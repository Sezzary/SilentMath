#pragma once

namespace Silent::Renderer
{
    /** @brief Per-object GPU uniform data for 3D model. */
    struct alignas(16) UniformModel
    {
        uint IsFastAlpha = false;
    };
}
