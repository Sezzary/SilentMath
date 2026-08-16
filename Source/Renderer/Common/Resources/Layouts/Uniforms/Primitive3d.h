#pragma once

namespace Silent::Renderer
{
    /** @brief Per-object GPU uniform data for 3D primitive. */
    struct alignas(16) UniformPrimitive3d
    {
        float ModelMat[4][4];
    };
}
