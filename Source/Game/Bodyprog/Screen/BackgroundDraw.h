#pragma once

namespace Silent::Game
{
    struct s_FsImageDesc;

    /** @brief Gamma intensity of background images. */
    extern q0_8 g_Screen_BackgroundImgGamma;

    void Screen_BackgroundImgDraw(const std::string& assetName, bool fit = true);

    /** @brief Draws a background image.
     *
     * @note Only applies to menus and the preview of maps when grabbing them.
     */
    void Screen_BackgroundImgDraw(s_FsImageDesc* image);

    void Screen_BackgroundImgTransition(s_FsImageDesc* image0, s_FsImageDesc* image1, q3_12 alpha);

    /** @brief Draws a background image.
     *
     * @note Only applies to background images of notes or puzzles images.
     */
    void Screen_BackgroundImgDrawAlt(s_FsImageDesc* image);

    /** @brief Applies motion blur to the scene.
     *
     * @note Used only in the loading screen.
     */
    bool Screen_BackgroundMotionBlur(s32 vBlanks);
}
