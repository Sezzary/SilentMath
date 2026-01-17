#pragma once

namespace Silent::Renderer
{
    constexpr auto SCREEN_SPACE_RES           = Vector2(100.0f, 100.0f);
    constexpr auto RETRO_SCREEN_SPACE_RES     = Vector2(320.0f, 240.0f);

    constexpr int DEBUG_GUI_COUNT_MAX = 8;

    constexpr int TRI_IDX_COUNT   = 3;
    constexpr int QUAD_IDX_COUNT  = 6;
    constexpr int RGBA_COMP_COUNT = 4;

    constexpr std::array<int, TRI_VERTEX_COUNT * 2> QUAD_TRI_IDXS = { 0, 1, 2, 0, 2, 3 };

    constexpr int DEPTH_MAX = 1024;
}
