#pragma once

namespace Silent::Renderer
{
    /** @brief GPU uniform buffer for 2D glyph shader. */
    struct UniformGlyph2d
    {
        uint  HasGradient    = false;
        uint  GradientSteps  = 0;
        float GradientUvMinY = 0.0f;
        float GradientUvMaxY = 0.0f;
    };
}
