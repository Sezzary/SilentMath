#pragma once

namespace Silent::Renderer
{
    /** @brief GPU uniform buffer for post-process fade shader. */
    struct UniformFade
    {
        float FadeAlpha = 0.0f;
    };
}
