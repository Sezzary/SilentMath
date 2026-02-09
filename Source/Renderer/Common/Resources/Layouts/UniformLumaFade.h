#pragma once

namespace Silent::Renderer
{
    /** @brief Per-frame GPU uniform data for post-process luma fade. */
    struct alignas(16) UniformLumaFade
    {
        float FadeAlpha = 0.0f;
    };
}
