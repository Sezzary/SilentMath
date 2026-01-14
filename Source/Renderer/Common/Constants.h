#pragma once

namespace Silent::Renderer
{
    constexpr auto SCREEN_SPACE_RES           = Vector2(100.0f, 100.0f);
    constexpr auto RETRO_SCREEN_SPACE_RES     = Vector2(320.0f, 240.0f);
    constexpr char SCREENSHOT_FILENAME_BASE[] = "Screenshot_";

    constexpr int PRIMITIVE_2D_COUNT_MAX        = 256;
    constexpr int SPRITE_2D_COUNT_MAX           = 256;
    constexpr int PRIMITIVE_3D_COUNT_MAX        = 256;
    constexpr int DEBUG_GUI_DRAW_CALL_COUNT_MAX = 8;

    constexpr int TRI_IDX_COUNT    = 3;
    constexpr int QUAD_INDEX_COUNT = 6;
    constexpr int RGBA_COMP_COUNT  = 4;
}
