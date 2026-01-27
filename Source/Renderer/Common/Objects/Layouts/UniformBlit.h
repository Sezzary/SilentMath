#pragma once

namespace Silent::Renderer
{
    /** @brief GPU uniform buffer for BLIT shader. */
    struct UniformBlit
    {
        float Brightness = 0.0f;
        float Contrast   = 0.0f;
    };
}
