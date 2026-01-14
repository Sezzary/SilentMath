#pragma once

namespace Silent::Renderer
{
    /** @brief 2D primitive shader uniform. */
    struct Primitive2dUniform
    {
        uint UseTexture  = false; /** `bool` */
        uint IsFastAlpha = false; /** `bool` */
    };
}
