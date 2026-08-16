#pragma once

namespace Silent::Renderer
{
    /** @brief Per-frame GPU uniform data for 3D primitive vertex shader. */
    struct alignas(16) UniformView
    {
        float ViewProjMat[4][4];
    };
}
