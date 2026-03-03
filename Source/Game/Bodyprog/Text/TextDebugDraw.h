#pragma once

namespace Silent::Game
{
    void Text_Debug_PositionSet(s16 x, s16 y);

    /** Draws debug strings using 8x8 glyphs from `FONT8.TIM`. Valid `char` range: `[42, 95]` (`*` to `_`). */
    void Text_Debug_Draw(char* str);

    char* Text_Debug_IntToString(s32 widthMin, s32 val);

    void func_800321EC(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
}
