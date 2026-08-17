#pragma once

namespace Silent::Renderer
{
    /** @brief Per-frame GPU uniform data for 3D primitive vertex shader. */
    struct alignas(16) UniformView
    {
        Matrix ViewProjMat;
        uint   HasJitter;
        float  ViewportAspectRatio;
    };
}
