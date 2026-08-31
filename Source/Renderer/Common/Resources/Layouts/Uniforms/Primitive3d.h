#pragma once

namespace Silent::Renderer
{
    /** @brief Per-frame GPU uniform data for 3D primitive. */
    struct alignas(16) UniformPrimitive3dPerFrame
    {
        Matrix ViewProjectionMat   = Matrix::Identity;
        uint   HasJitter           = false;
        float  ViewportAspectRatio = 1.0f;
    };

    /** @brief Per-object GPU uniform data for 3D primitive. */
    struct alignas(16) UniformPrimitive3dPerObject
    {
        Matrix ModelMat = Matrix::Identity;
    };
}
