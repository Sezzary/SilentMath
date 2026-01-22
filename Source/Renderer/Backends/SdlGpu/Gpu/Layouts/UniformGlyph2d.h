#pragma once

namespace Silent::Renderer
{
    /** @brief GPU uniform buffer for 2D glyph shader. */
    struct UniformGlyph2d
    {
        float UvMinY        = 0.0f;
        float UvMaxY        = 0.0f;
        float Center        = 0.0f;
        uint  GradientSteps = false;
        //--
        uint  HasGradient   = false;
    };
}
