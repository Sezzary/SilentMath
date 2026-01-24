#pragma once

namespace Silent::Game
{
    #define DEFAULT_MAP_MESSAGE_LENGTH     99

    /** @brief String color IDs for strings displayed in screen space.
     * Used as indices into `STRING_COLORS`.
     */
    enum class e_StringColorId
    {
        Gold,
        DarkGrey,
        Green,
        Nuclear,
        Red,
        LightGrey,
        White,
        Black, // @todo Need to add this one for paper maps.

        Count
    };

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
}
