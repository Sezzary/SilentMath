#pragma once

namespace Silent::Renderer
{
    /** @brief GPU uniform buffer for 3D primitive vertex shader. */
    struct alignas(16) UniformPrimitive3d
    {
        float ModelMat[4][4];
        float ViewProjMat[4][4];
        //Matrix ModelMat    = Matrix::Identity;
        //Matrix ViewProjMat = Matrix::Identity;
    };
}
