#pragma once

namespace Silent::Renderer
{
    /** @brief GPU uniform buffer for 2D glyph shader. */
    struct UniformGlyph2d
    {
        Vector2 UvMin       = Vector2::Zero;
        Vector2 UvMax       = Vector2::Zero;
        //--
        float   Center      = 0.0f;
        uint    HasGradient = false; /** `bool` */
        uint    IsRetro     = false; /** `bool` */
    };
}
