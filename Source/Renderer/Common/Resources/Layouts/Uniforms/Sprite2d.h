#pragma once

namespace Silent::Renderer
{
    /** @brief Per-object GPU uniform data for 2D sprite. */
    struct alignas(16) UniformSprite2dPerObject
    {
        uint IsFastAlpha = false;
    };
}
