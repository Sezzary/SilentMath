#pragma once

namespace Silent::Renderer
{
    /** @brief GPU uniform buffer for 3D primitive vertex shader. */
    struct alignas(16) UniformPrimitive3d
    {
        Matrix ModelMat    = Matrix::Identity;
        Matrix ViewProjMat = Matrix::Identity;
    };
}
