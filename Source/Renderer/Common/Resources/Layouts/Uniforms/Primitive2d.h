#pragma once

namespace Silent::Renderer
{
    /** @brief Per-frame GPU uniform data for 2D primitive. */
    struct alignas(16) UniformPrimitive2dPerFrame
    {
        Matrix ProjectionMat       = Matrix::Identity;
        uint   HasJitter           = false;
        float  ViewportAspectRatio = 1.0f;
    };

    /** @brief Per-object GPU uniform data for 2D primitive. */
    struct alignas(16) UniformPrimitive2dPerObject
    {
        Matrix ModelMat = Matrix::Identity;
    };
}
