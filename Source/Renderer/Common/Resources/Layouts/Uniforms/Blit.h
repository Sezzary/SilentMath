#pragma once

namespace Silent::Renderer
{
    /** @brief Per-frame GPU uniform data for BLIT data. */
    struct alignas(16) UniformBlit
    {
        float Brightness = 0.0f;
    };
}
