#include "Framework.h"
#include "Services/Message.h"

#include "Application.h"
#include "Renderer/Common/Enums.h"
#include "Renderer/Renderer.h"

using namespace Silent::Renderer;

namespace Silent::Services
{
    constexpr auto MSG_CODE_OPEN          = U'{';
    constexpr auto MSG_CODE_CLOSE         = U'}';
    constexpr auto MSG_CODE_LEADING       = U'A'; /** Line leading. */
    constexpr auto MSG_CODE_COLOR         = U'C'; /** Set color. */
    constexpr auto MSG_CODE_DISPLAY_ALL   = U'D'; /** Display message instantly without roll. */
    constexpr auto MSG_CODE_END           = U'E'; /** End dialog message. */
    constexpr auto MSG_CODE_HIGH_RES      = U'H'; /** High-resolution glyph drawing. */
    constexpr auto MSG_CODE_JUMP          = U'J'; /** Jump timer. */
    constexpr auto MSG_CODE_KERNING       = U'K'; /** Character kerning. */
    constexpr auto MSG_CODE_LINE_POSITION = U'L'; /** Set next line position. */
    constexpr auto MSG_CODE_MIDDLE        = U'M'; /** Align center. */
    constexpr auto MSG_CODE_NEWLINE       = U'N'; /** Newline. */
    constexpr auto MSG_CODE_SELECT        = U'S'; /** Display dialog prompt with selectable entries. */
    constexpr auto MSG_CODE_TAB           = U'T'; /** Inset line. */

    std::vector<char32> GetCodePoints(const std::string& msg)
    {
        // Reserve minimum size.
        auto codePoints = std::vector<char32>{};
        codePoints.reserve((msg.size() / 4) + 1);

        // Collect code points.
        utf8::utf8to32(msg.begin(), msg.end(), std::back_inserter(codePoints));
        return codePoints;
    }

    void SubmitMessage(const std::string& msg, const Vector2& pos, const std::string& font, TextStyle style, float scale, int length)
    {
        auto& renderer = g_App.GetRenderer();

        // @todo

        auto   curPos        = pos;
        char32 activeCommand = NO_VALUE;
        auto   msgRun        = std::string();

        // Parse message.
        auto codePoints = GetCodePoints(msg);
        for (char32 codePoint : codePoints)
        {
            switch (codePoint)
            {
                case MSG_CODE_OPEN:
                {
                    break;
                }
                case MSG_CODE_CLOSE:
                {
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
    }
}
