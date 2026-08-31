#include "Framework.h"
#include "Psx.h"
#include "Game/Bodyprog/Text/TextDraw.h"

#include "Game/Bodyprog/Bodyprog.h"

#include "Application.h"
#include "Renderer/Renderer.h"
#include "Renderer/Common/Enums.h"
#include "Renderer/Common/Resources/Scene/Text2d.h"
#include "Renderer/Common/Utils.h"
#include "Utils/Font.h"
#include "Utils/Utils.h"

using namespace Silent::Renderer;
using namespace Silent::Utils;

namespace Silent::Game
{
    /** @brief Message line position types for `MAP_MSG_CODE_LINE_POSITION` arguments. */
    enum class MsgLinePositionType
    {
        Subtitle    = 0,
        Information = 4,

        Count
    };

    constexpr auto STRING_COLORS = std::array<Color, StringColorId_Count>
    {
        Color::From8Bit(160, 128, 64),
        Color::From8Bit(32,  32,  32),
        Color::From8Bit(24,  128, 40),
        Color::From8Bit(8,   184, 96),
        Color::From8Bit(128, 0,   0),
        Color::From8Bit(24,  128, 40),
        Color::From8Bit(100, 100, 100),
        Color::From8Bit(255, 255, 255), // @todo Originally `(128, 128, 128)`. Need to adjust the others too?
        Color::From8Bit(4,   4,   4)
    };

    Vector2i g_StringPosition;
    u8       g_MapMsg_AudioLoadBlock;

    static auto g_StringColorId  = StringColorId_White;
    
    ParsedMsg GetParsedMsg(const std::string& msg, const std::string& fontName, float lineHeight)
    {
        auto& fonts = g_App.GetFonts();

        // Get font.
        auto* font = fonts.GetFont(fontName);
        if (font == nullptr)
        {
            Debug::Log(Fmt("Attempted to parse tagged message with invalid font `{}`.", fontName),
                       Debug::LogLevel::Error);
            return {};
        }

        auto buffer = std::string();
        bool inCmd  = false;

        // Parse message into nodes.
        auto nodes = std::vector<MsgNode>{};
        for (char c : msg)
        {
            // Start collecting command.
            if (c == '{' && !inCmd)
            {
                if (!buffer.empty())
                {
                    nodes.push_back(MsgNode
                    {
                        .Type  = NodeType::Text,
                        .Value = buffer
                    });

                    buffer.clear();
                }

                inCmd = true;
            }
            // Finish collecting command.
            else if (c == '}' && inCmd)
            {
                nodes.push_back(MsgNode
                {
                    .Type  = NodeType::Command,
                    .Value = buffer
                });

                buffer.clear();
                inCmd = false;
            }
            // Collect `char`s.
            else
            {
                buffer += c;
            }
        }

        // Compute line widths.
        auto lineWidths = std::vector<float>{};
        for (const auto& node : nodes)
        {
            // Ignore non-text node.
            if (node.Type != NodeType::Text)
            {
                continue;
            }

            // Collect line width.
            auto shape = font->GetShapedText(node.Value);
            lineWidths.push_back(shape.Width);
        }

        return ParsedMsg
        {
            .FontName   = fontName,
            .Nodes      = std::move(nodes),
            .LineWidths = std::move(lineWidths),
            .LineHeight = lineHeight
        };
    }

    void DrawString(const std::string& str, const std::string& fontName, const Vector2& pos, float scale,
                    const Color& color, int styleFlags, AlignMode alignMode)
    {
        constexpr int DEPTH = 6;

        const auto& options  = g_App.GetOptions();
        auto&       renderer = g_App.GetRenderer();

        // Submit text.
        //auto pos  = GetGridAlignedScreenPercent(ConvertRetroScreenPixelsToPercent(g_StringPosition), (int)RETRO_SCREEN_SPACE_RES.y);
        auto text = Text2d::CreateText2d(str, fontName,
                                         pos, 0.0f, scale, 1.0f,
                                         color, styleFlags,
                                         DEPTH, alignMode);
        renderer.SubmitText2d(text);
    }

    e_MsgReturnCode DrawParsedMsg(const ParsedMsg& msg, const Vector2& pos, float scale,
                                  int styleFlags, int displayLength)
    {
        constexpr int TAB_SIZE = (40.0f / RETRO_SCREEN_SPACE_RES.x) * SCREEN_SPACE_RES.x;

        struct State
        {
            Vector2         Position   = Vector2::Zero;
            Vector2         Offset     = Vector2::Zero;
            float           LineHeight = 0.0f;
            e_StringColorId ColorId    = StringColorId_White;
            int             StyleFlags = (int)TextStyleFlags::None;
            AlignMode       AlignMd    = AlignMode::BottomLeft;
        };

        auto state = State
        {
            .Position   = pos,
            .LineHeight = msg.LineHeight,
            .StyleFlags = styleFlags
        };
        auto returnCode = MsgReturnCode_None;

        // Run through message nodes.
        for (const auto& node : msg.Nodes)
        {
            // Skip invalid node.
            if (node.Value.empty())
            {
                continue;
            }

            // Draw text.
            if (node.Type == NodeType::Text)
            {
                // Draw text string.
                int  glyphCount = GetCodePoints(node.Value).size();
                auto str        = node.Value.substr(0, std::min(glyphCount, displayLength - 1));
                DrawString(str, msg.FontName, state.Position, scale,
                           STRING_COLORS[state.ColorId], state.StyleFlags,
                           state.AlignMd);

                // Stop drawing if length exceeded.
                displayLength -= glyphCount;
                if (displayLength <= 0)
                {
                    return MsgReturnCode_None;
                }
            }
            // Handle command.
            else if (node.Type == NodeType::Command)
            {
                char code = node.Value[0];
                int  arg  = node.Value[1]; // @note Note valid for `MAP_MSG_CODE_JUMP`.

                switch (code)
                {
                    case MAP_MSG_CODE_COLOR:
                    {
                        state.ColorId = (e_StringColorId)arg;
                        break;
                    }
                    case MAP_MSG_CODE_END:
                    {
                        returnCode = MsgReturnCode_End;
                        break;
                    }
                    case MAP_MSG_CODE_HALF_HEIGHT:
                    {
                        state.StyleFlags &= (int)TextStyleFlags::HalfHeight;
                        state.LineHeight /= 2;
                        break;
                    }
                    case MAP_MSG_CODE_JUMP:
                    {
                        // @todo What do these values mean? This was only set when computing line widths in legacy code.
                        if (arg == 2)
                        {
                            g_MapMsg_AudioLoadBlock = 3;
                        }

                        // Skip if another message is already in progress.
                        if (g_SysWork.mapMsgTimer != NO_VALUE)
                        {
                            break;
                        }

                        // Parse time value, e.g. `(1.5)`.
                        auto timeStr = std::string();
                        for (char c : node.Value.substr(2, node.Value.size() - 1))
                        {
                            // Reached end.
                            if (c == ')')
                            {
                                break;
                            }

                            timeStr += c;
                        }

                        // Set map message timer.
                        float time            = std::stof(timeStr);
                        g_SysWork.mapMsgTimer = Q12(time);
                        break;
                    }
                    case MAP_MSG_CODE_LINE_POSITION:
                    {
                        switch ((MsgLinePositionType)arg)
                        {
                            case MsgLinePositionType::Subtitle:
                            {
                                // Compute widest line width.
                                int widestLineWidth = 0;
                                for (int lineWidth : msg.LineWidths)
                                {
                                    if (lineWidth > widestLineWidth)
                                    {
                                        widestLineWidth = lineWidth;
                                    }
                                }

                                // Set start line position. @todo To percent.
                                state.Position = Vector2((int)round((120 - (widestLineWidth / 2)) * GetScreenAspectCorrection(GLYPH_SCALE_MODE).x),
                                                          (320 - (((msg.LineWidths.size() - 1) * state.LineHeight))) - 8);
                                break;
                            }
                            case MsgLinePositionType::Information:
                            {
                                state.Position = ConvertRetroScreenPixelsToPercent(Vector2i(160, 76));
                                break;
                            }
                        }
                        break;
                    }
                    case MAP_MSG_CODE_ALIGN_CENTER:
                    {
                        state.AlignMd = AlignMode::BottomLeft;
                        break;
                    }
                    case MAP_MSG_CODE_NEWLINE:
                    {
                        state.Offset.y += state.LineHeight;
                        break;
                    }
                    case MAP_MSG_CODE_SELECT:
                    {
                        returnCode = (e_MsgReturnCode)arg;
                        break;
                    }
                    case MAP_MSG_CODE_TAB:
                    {
                        state.Offset.x += TAB_SIZE;
                        break;
                    }
                    case MAP_MSG_CODE_END_PAGE:
                    {
                        returnCode = MsgReturnCode_EndPage;
                        break;
                    }
                    case MAP_MSG_CODE_ALIGN_RIGHT:
                    {
                        state.AlignMd = AlignMode::BottomRight;
                        break;
                    }
                }
            }
        }

        return returnCode;
    }

    void Gfx_StringPositionSet(int posX, int posY)
    {
        if (posX != NO_VALUE)
        {
            g_StringPosition.x = posX;
        }

        if (posY != NO_VALUE)
        {
            g_StringPosition.y = posY;
        }
    }

    void Gfx_StringColorSet(e_StringColorId colorId)
    {
        g_StringColorId = colorId;
    }

    void Gfx_MapMsg_Reset()
    {
        g_StringColorId = StringColorId_White;
    }

    void Gfx_StringDraw(const std::string& str, int displayLength, bool isHalfHeight, AlignMode alignMode)
    {
        constexpr float SCALE = RETRO_PIXEL_SCALE.y * 16.0f;

        const auto& options  = g_App.GetOptions();
        auto&       renderer = g_App.GetRenderer();

        // Draw string.
        auto fontName   = (options->TextQuality == TextQualityType::Retro) ? "RetroSerif" : "ModernSerif";
        auto pos        = ConvertRetroScreenPixelsToPercent(g_StringPosition);//GetGridAlignedScreenPercent(ConvertRetroScreenPixelsToPercent(g_StringPosition), (int)RETRO_SCREEN_SPACE_RES.y);
        int  styleFlags = (int)TextStyleFlags::Gradient |
                          (int)TextStyleFlags::Shadow   |
                          (isHalfHeight ? (int)TextStyleFlags::HalfHeight : (int)TextStyleFlags::None);
        DrawString(str, fontName, pos, SCALE, STRING_COLORS[g_StringColorId], styleFlags, alignMode);
    }

    void Gfx_StringDrawInt(s32 widthMin, s32 displayLength)
    {
        Gfx_StringDraw(std::to_string(displayLength), 5);
    }
}
