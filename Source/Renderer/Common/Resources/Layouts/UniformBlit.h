#pragma once

namespace Silent::Renderer
{
    /** @brief GPU uniform buffer for BLIT shader. */
    struct alignas(16) UniformBlit
    {
        float Brightness = 0.0f;
    };
}
