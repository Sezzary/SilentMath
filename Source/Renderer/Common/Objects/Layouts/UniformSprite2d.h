#pragma once

namespace Silent::Renderer
{
    /** @brief GPU uniform buffer for 2D sprite shader. */
    struct UniformSprite2d
    {
        uint UseTexture  = false;
        uint IsFastAlpha = false;
    };
}
