#pragma once

namespace Silent::Renderer
{
    /** @brief Per-frame GPU uniform data for view in 3D primitive vertex. */
    struct alignas(16) UniformView
    {
        Matrix ViewProjMat         = Matrix::Identity;
        uint   HasJitter           = false;
        float  ViewportAspectRatio = 1.0f;
    };
}
