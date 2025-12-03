#pragma once

namespace Silent::Renderer
{
    enum class ScaleMode;

    /** @brief Computes the screen aspect ratio correction scale according to a provided scale mode.
     *
     * @param scaleMode Renderer scale mode.
     * @return Screen aspect correction.
     */
    Vector2 GetScreenAspectCorrection(ScaleMode scaleMode);

    /** @brief Computes a screen position in percent corrected according to the screen aspect ratio and a scale mode.
     *
     * @param pos Screen position in percent.
     * @param scaleMode Scale mode.
     * @return Screen position in percent corrected for the screen aspect ratio and scale mode.
     */
    Vector2 GetAspectCorrectScreenPosition(const Vector2 pos, ScaleMode scaleMode);

    /** @brief Converts a screen position in retro pixels to percent.
     *
     * @param pos Screen position in retro pixels (320x240 resolution).
     * @return Screen position in percent.
     */
    Vector2 ConvertRetroScreenPixelsToPercent(const Vector2i& pos);

    /** @brief Converts a position in percent to retro screen pixels.
     *
     * @param pos Screen position in retro screen pixels (320x240 resolution).
     * @return Screen position in percent.
     */
    Vector2i ConvertScreenPercentToRetroPixels(const Vector2& pos);

    /** @brief Converts a screen position in percent to normalized device coordinates.
     *
     * @param pos Screen position in percent.
     * @return Normalized device coordinates (viewable range `[-1.0f, 1.0f]`).
     */
    Vector2 ConvertScreenPercentToNdc(const Vector2& pos);

    /** @brief Converts normalized device coordinates to a screen position in percent.
     *
     * @param ndc Normalized device coordinates (viewable range `[-1.0f, 1.0f]`).
     * @return Screen position in percent.
     */
    Vector2 ConvertNdcToScreenPercent(const Vector2& ndc);

    /** @brief Normalizes retro screen coordinates for negative values, which are offsets from the center.
     *
     * @todo Not always correct. Should always convert positions manually when porting original negative screen position values.
     *
     * @param pos Screen position in retro pixels (320x240 resolution).
     * @return Screen position in retro screen coordinates normalized for negative values.
     */
    Vector2i NormalizeRetroScreenPosition(const Vector2i pos);
}
