#pragma once

namespace Silent::Renderer{ enum class TextStyle; }

using namespace Silent::Renderer;

namespace Silent::Services
{
    constexpr int DEFAULT_MAP_MSG_LENGTH     = 99;
    constexpr int MAP_MSG_DISPLAY_ALL_LENGTH = 400; /** Long string length is used to display a whole message instantly without a rollout. */

    /** @brief Submits a message for the renderer to draw in screen space.
     *
     * @param msg Message to print.
     * @param pos Screen position in percent.
     * @param font Font name.
     * @param style Text embellishment style.
     * @param scale Scale relative to the shortest screen axis.
     * @param length Message length.
     */
    void SubmitMessage(const std::string& msg, const Vector2& pos, const std::string& font, TextStyle style, float scale, int length = MAP_MSG_DISPLAY_ALL_LENGTH);
}
