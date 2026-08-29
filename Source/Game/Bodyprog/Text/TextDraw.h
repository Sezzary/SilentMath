#pragma once

#include "Renderer/Common/Enums.h"

using namespace Silent::Renderer;

namespace Silent::Game
{
    constexpr char MAP_MSG_CODE_COLOR         = 'C'; /** Set color. */
    constexpr char MAP_MSG_CODE_DISPLAY_ALL   = 'D'; /** Display message instantly with no rollout. */
    constexpr char MAP_MSG_CODE_END           = 'E'; /** End message. */
    constexpr char MAP_MSG_CODE_HALF_HEIGHT   = 'H'; /** Half-height glyphs. */
    constexpr char MAP_MSG_CODE_JUMP          = 'J'; /** Jump timer. */
    constexpr char MAP_MSG_CODE_LINE_POSITION = 'L'; /** Set next line position. */
    constexpr char MAP_MSG_CODE_ALIGN_CENTER  = 'M'; /** Align center. */
    constexpr char MAP_MSG_CODE_NEWLINE       = 'N'; /** Newline. */
    constexpr char MAP_MSG_CODE_SELECT        = 'S'; /** Display dialog prompt with selectable entries. */
    constexpr char MAP_MSG_CODE_TAB           = 'T'; /** Inset line. */
    constexpr char MAP_MSG_CODE_PAGE          = 'P'; /** Split page. */ // @todo New.
    constexpr char MAP_MSG_CODE_ALIGN_RIGHT   = 'R'; /** Align right. */

    constexpr int FONT_12X16_GLYPH_SIZE_Y   = 16;
    constexpr int FONT_12X16_LINE_COUNT_MAX = 9;

    /** @brief Processed message node types. */
    enum class NodeType
    {
        Text,
        Command
    };

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
        StringColorId_Black       = 8,

        StringColorId_Count
    };

    /** @brief Processed message node. */
    struct MsgNode
    {
        NodeType    Type  = NodeType::Text;
        std::string Value = {};
    };

    /** Used in string parsing. */
    struct s_800C38B0
    {
        s8 field_0;
        u8 positionIdx;
    };

    extern s32      g_PositionIdx;
    extern s32      g_MapMsg_WidthIdx;
    extern s32      g_MapMsg_Widths[12];
    extern Vector2i g_StringPosition;
    extern int      g_StringPositionX1;
    extern s32      g_MapMsg_WidthIdx;
    extern s32      g_MapMsg_Widths[12];

    /** @brief Sets the global position of the next string to be drawn by `Gfx_StringDraw`.
     *
     * @param x X screen position.
     * @param y Y screen position.
     */
    void Gfx_StringSetPosition(int posX, int posY);

    void Gfx_Strings2dLayerIdxSet(s32 idx);

    void Gfx_StringsReset2dLayerIdx();

    /** @brief Sets the global color state of the next string drawn by `Gfx_StringDraw`.
     *
     * @param colorId ID of the new color to set (`e_ColorId`).
     */
    void Gfx_StringColorSet(e_StringColorId colorId);

    /** @brief Draws a string in screen space using 12x16 glyphs.
     *
     * @note References glyphs in `FONT16.TIM`. The texture is loaded into VRAM across multiple texture pages,
     * hence why the texture is a single row with 4-pixel padding every 21st glyph instead of a stacked arrangement.
     *
     * @param str String to draw.
     * @param strLength Number of consecutive glyphs to draw from the string.
     */
    float Gfx_StringDraw(const std::string& str, int strLength, bool isHalfHeight = false, AlignMode alignMode = AlignMode::BottomLeft);

    /** @brief Computes the screen space widths of lines in a map message using 12x16 glyphs and populates
     * `g_MapMsg_Widths`.
     *
     * @param mapMsgIdx Index of the map message to evaluate.
     */
    s32 Gfx_MapMsg_WidthsCompute(s32 mapMsgIdx);

    /** @brief Draws a string in screen space using 12x16 glyphs and returns a map message code.
     *
     * @param mapMsg Map message to draw.
     * @param strLength Number of consecutive glyphs to draw from the map message.
     * @return Map message code (`e_MapMsgReturnCode`).
     */
    s32 Gfx_MapMsg_StringDraw(const std::string& mapMsg, s32 strLength, bool isHalfHeight = false);

    void Gfx_MapMsg_DefaultStringInfoSet();

    /** @brief Draws an integer string in screen space using 12x16 glyphs.
     *
     * @param widthMin Minimum width of the integer string.
     * @param strLength Number of consecutive glyphs to draw from the integer string.
     */
    void Gfx_StringDrawInt(s32 widthMin, s32 strLength);
}
