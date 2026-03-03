#pragma once

namespace Silent::Game
{
    /** @brief String color IDs for strings displayed in screen space.
     * Used as indices into `STRING_COLORS`.
     */
    enum e_StringColorId
    {
        StringColorId_Gold,
        StringColorId_DarkGrey,
        StringColorId_Green,
        StringColorId_Nuclear,
        StringColorId_Red,
        StringColorId_LightGrey,
        StringColorId_White,
        //StringColorId_Black, // @todo Need to add this one for paper maps.

        StringColorId_Count
    };

    /** Used in string parsing. */
    struct s_800C38B0
    {
        s8 field_0;
        s8 positionIdx_1;
    };

    extern s_800C38B0 D_800C38B0;

    extern s32 D_800C38B4;

    extern s32 g_MapMsg_WidthTable[12];

    /** String glyph sprite. */
    extern GsSPRITE D_800C38F8;

    extern s16 D_800C391C;

    extern s32 D_800C3920;

    /** String position. */
    extern Vector2i g_StringPosition;

    /** String X position. It's unclear why there are two. */
    extern int g_StringPositionX1;

    /** Sets the position of the next string to be drawn by `Gfx_StringDraw`. */
    void Gfx_StringSetPosition(int posX, int posY);

    /** Sets the color of the next string drawn by `Gfx_StringDraw`.
     *
     * @param colorId ID of the color to set.
     */
    void Gfx_StringSetColor(e_StringColorId colorId);

    /** Draws a string in screen space using 12x16 glyphs.
     *
     * @param str String to draw.
     * @param strLength String length for rollout.
     */
    bool Gfx_StringDraw(const std::string& str, int strLength);

    void Gfx_StringDrawInt(s32 widthMin, s32 strLength);
}
