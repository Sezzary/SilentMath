#pragma once

namespace Silent::Renderer
{
    constexpr auto  SCREEN_SPACE_RES           = Vector2(100.0f, 100.0f);
    constexpr auto  RETRO_SCREEN_SPACE_RES     = Vector2(320.0f, 240.0f);
    constexpr auto  RETRO_PIXEL_SCALE          = Vector2(1.0f / RETRO_SCREEN_SPACE_RES.x, 1.0f / RETRO_SCREEN_SPACE_RES.y);
    constexpr float ASPECT_RATIO_4_TO_3        = 4.0f  / 3.0f;
    constexpr float ASPECT_RATIO_16_TO_9       = 16.0f / 9.0f;
    constexpr int   VIRTUAL_SCREEN_HEIGHT_240P = 240;
    constexpr int   VIRTUAL_SCREEN_HEIGHT_480P = 480;

    constexpr int DEPTH_2D_MAX = 1024;

    constexpr int  DEFAULT_TEXTURE_SIZE   = 4;
    constexpr auto DEFAULT_TEXTURE_RES    = Vector2i(DEFAULT_TEXTURE_SIZE, DEFAULT_TEXTURE_SIZE);
    constexpr auto DEFAULT_TEXTURE_PIXELS = std::array<byte, SQUARE(DEFAULT_TEXTURE_SIZE) * RGBA_COMP_COUNT>
    {
        Q8_COLOR(0.0f), Q8_COLOR(0.0f), Q8_COLOR(0.0f), Q8_COLOR(1.0f),
        Q8_COLOR(1.0f), Q8_COLOR(0.0f), Q8_COLOR(1.0f), Q8_COLOR(1.0f),
        Q8_COLOR(1.0f), Q8_COLOR(0.0f), Q8_COLOR(1.0f), Q8_COLOR(1.0f),
        Q8_COLOR(0.0f), Q8_COLOR(0.0f), Q8_COLOR(0.0f), Q8_COLOR(1.0f)
    };

    constexpr int DEBUG_GUI_COUNT_MAX = 8;
}
