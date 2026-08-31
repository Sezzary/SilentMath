#pragma once

namespace Silent::Renderer
{
    /** @brief Per-object GPU uniform data for 2D glyph. */
    struct alignas(16) UniformGlyph2dPerObject
    {
        uint  HasGradient    = false;
        float GradientUvMinY = 0.0f;
        float GradientUvMaxY = 0.0f;
    };
}
