#pragma once

namespace Silent::Renderer
{
    /** @brief GPU uniform buffer for 2D triangle shader. */
    struct UniformTriangle2d
    {
        uint UseTexture  = false; /** `bool` */
        uint IsFastAlpha = false; /** `bool` */
    };
}
