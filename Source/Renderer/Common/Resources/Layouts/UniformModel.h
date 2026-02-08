#pragma once

namespace Silent::Renderer
{
    /** @brief GPU uniform buffer for 3D model fragment shader. */
    struct alignas(16) UniformModel
    {
        uint UseTexture  = false;
        uint IsFastAlpha = false;
    };
}
