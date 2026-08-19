#pragma once

namespace Silent::Renderer
{
    /** @brief Per-frame GPU uniform data for luma fade post-process. */
    struct alignas(16) UniformLumaFade
    {
        float FadeAlpha = 0.0f;
        uint  IsWhite   = false;
    };
}
