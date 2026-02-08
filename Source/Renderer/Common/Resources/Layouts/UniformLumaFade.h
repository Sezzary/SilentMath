#pragma once

namespace Silent::Renderer
{
    /** @brief GPU uniform buffer for post-process luma fade fragment shader. */
    struct alignas(16) UniformLumaFade
    {
        float FadeAlpha = 0.0f;
    };
}
