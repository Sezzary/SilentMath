#pragma once

namespace Silent::Game
{

    constexpr char MAP_MSG_CODE_COLOR         = 'C'; /** Set color. */
    constexpr char MAP_MSG_CODE_DISPLAY_ALL   = 'D'; /** Display message instantly with no rollout. */
    constexpr char MAP_MSG_CODE_END           = 'E'; /** End message. */
    constexpr char MAP_MSG_CODE_JUMP          = 'J'; /** Jump timer. */
    constexpr char MAP_MSG_CODE_LINE_POSITION = 'L'; /** Set next line position. */
    constexpr char MAP_MSG_CODE_MIDDLE        = 'M'; /** Align center. */
    constexpr char MAP_MSG_CODE_NEWLINE       = 'N'; /** Newline. */
    constexpr char MAP_MSG_CODE_SELECT        = 'S'; /** Display dialog prompt with selectable entries. */
    constexpr char MAP_MSG_CODE_TAB           = 'T'; /** Inset line. */

    constexpr int FONT_12X16_GLYPH_SIZE_Y   = 16;
    constexpr int FONT_12X16_LINE_COUNT_MAX = 9;

    /** @brief String color IDs for strings displayed in screen space.
     * Used as indices into `STRING_COLORS`.
     */
    enum e_StringColorId
    {
        StringColorId_Gold        = 0,
        StringColorId_DarkGrey    = 1,
        StringColorId_Green       = 2,
        StringColorId_Nuclear     = 3,
        StringColorId_Red         = 4,
        StringColorId_GreenUnused = 5, // @unused Same as `StringColorId_Green`.
        StringColorId_LightGrey   = 6,
        StringColorId_White       = 7,
        //StringColorId_Black   = 8, // @todo Need to add this one for paper maps.

        StringColorId_Count
    };

    /** Used in string parsing. */
    struct s_800C38B0
    {
        s8 field_0;
        u8 positionIdx_1;
    };

    extern s_800C38B0 D_800C38B0;

    extern s32 g_MapMsg_WidthIdx;

    extern s32 g_MapMsg_Widths[12];

    /** String glyph sprite. */
    extern GsSPRITE g_MapMsg_GlyphSprite;

    extern s16 D_800C391C;

    extern s32 D_800C3920;

    /** String position. */
    extern Vector2i g_StringPosition;

    /** String X position. It's unclear why there are two. */
    extern int g_StringPositionX1;

    extern s32 g_MapMsg_WidthIdx;

    extern s32 g_MapMsg_Widths[12];

    /** Sets the position of the next string to be drawn by `Gfx_StringDraw`. */
    void Gfx_StringSetPosition(int posX, int posY);

    void Gfx_Strings2dLayerIdxSet(s32 idx);

    void Gfx_StringsReset2dLayerIdx();

    /** Sets the color of the next string drawn by `Gfx_StringDraw`.
     *
     * @param colorId ID of the color to set.
     */
    void Gfx_StringSetColor(e_StringColorId colorId);

    /** Draws a string in screen space using 12x16 glyphs.
     *
     * @param str String to draw.
     * @param strLength String length for rollout
     * @return Length of the string.
     */
    float Gfx_StringDraw(const std::string& str, int strLength);

    s32 Gfx_MapMsg_CalculateWidths(s32 mapMsgIdx);

    /** Draws string and returns map message index. */
    s32 Gfx_MapMsg_StringDraw(char* mapMsg, s32 strLength);

    void func_8004B658();

    void Gfx_MapMsg_DefaultStringInfoSet();

    /** @unused */
    void func_8004B6D4(s16 arg0, s16 arg1);

    /** @unused */
    void func_8004B74C(s16 arg0);

    /** @unused Draws string. */
    void func_8004B76C(char* str, bool useFixedWidth);

    void Gfx_StringDrawInt(s32 widthMin, s32 strLength);

    void func_8004BB10();
}
