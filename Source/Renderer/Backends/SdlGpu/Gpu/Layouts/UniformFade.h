#pragma once

namespace Silent::Renderer
{
    /** @brief GPU uniform buffer for post-process fade shader. */
    struct UniformFade
    {
        float BlendAlpha = 0.0f;
    };
}
