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

    /** @brief Computes a screen position in percent corrected according to the aspect ratio of the screen and a scale mode.
     *
     * @param pos Screen position in percent.
     * @param scaleMode Scale mode.
     * @return Screen position in percent corrected for the screen aspect ratio and scale mode.
     */
    Vector2 GetAspectCorrectScreenPosition(const Vector2 pos, ScaleMode scaleMode);

    /** @brief Converts a screen position in percent to retro screen coodinates.
     *
     * @param pos Screen position in percent.
     * @return Retro screen coordinates (320x240).
     */
    Vector2 ConvertRetroScreenCoordsToScreenPosition(const Vector2i& pos);

    /** @brief Converts a position in screen percent to retro screen coodinates.
     *
     * @param pos Retro screen coordinates (320x240).
     * @return Screen position in percent.
     */
    Vector2i ConvertScreenPositionToRetroScreenCoords(const Vector2& pos);

    /** @brief Converts a screen position in percent to normalized device coordinates.
     *
     * @param pos Screen position in percent with components in the range `[0.0f, 100.0f]`.
     * @return Normalized device coordinates with components in the range `[-1.0f, 1.0f]`.
     */
    Vector2 ConvertScreenPositionToNdc(const Vector2& pos);

    /** @brief Converts normalized device coordinates to a screen position in percent.
     *
     * @param ndc Normalized device coordinates with components in the range `[-1.0f, 1.0f]`.
     * @return Screen position in percent with components in the range `[0.0f, 100.0f]`.
     */
    Vector2 ConvertNdcToScreenPosition(const Vector2& ndc);

    /** @brief Normalizes retro screen coordinates for negative values, which are offsets from the center.
     *
     * @param pos Retro screen coordinates (320x240).
     * @return Retro screen coordinates normalized for negative values.
     */
    Vector2i NormalizeRetroScreenPosition(const Vector2i pos);
}
