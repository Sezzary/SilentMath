#include "Framework.h"
#include "Psx.h"
#include "Game/Bodyprog/Text/TextDraw.h"

#include "Game/Bodyprog/Bodyprog.h"

#include "Application.h"
#include "Game/Bodyprog/Events/MapMsg.h"
#include "Renderer/Renderer.h"
#include "Renderer/Common/Enums.h"
#include "Renderer/Common/Resources/Scene/Text2d.h"
#include "Renderer/Common/Utils.h"
#include "Utils/Utils.h"

using namespace Silent::Renderer;
using namespace Silent::Utils;

namespace Silent::Game
{
    /** @brief Converts a floating-point X screen position in percent to a fixed-point X screen coodinate. */
    #define SCREEN_POSITION_X(percent) \
        (s32)((int)RETRO_SCREEN_SPACE_RES.x * ((percent) / 100.0f))

    /** @brief Converts a floating-point Y screen position in percent to a fixed-point Y screen coodinate. */
    #define SCREEN_POSITION_Y(percent) \
        (s32)((int)RETRO_SCREEN_SPACE_RES.y * ((percent) / 100.0f))

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

    static auto g_StringColorId = StringColorId_White;

    /** Text index 2D layer.
     * If modifying `Gfx_StringSetPosition`, when setting it to
     * a value lower than 6, text will not be affected by the fade effect.
     */
    static s32 g_Strings2dLayerIdx = 6;

    Vector2i g_StringPosition;
    int      g_StringPositionX1;
    s32      g_PositionIdx;
    s32      g_MapMsg_Widths[12];
    s32      g_MapMsg_WidthIdx;

    static std::vector<MsgNode> ParseMsg(const std::string& mapMsg)
    {
        auto nodes  = std::vector<MsgNode>{};
        auto buffer = std::string();
        bool inCmd  = false;

        // Parse message into nodes.
        for (char c : mapMsg)
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

        return nodes;
    }

    void Gfx_StringSetPosition(int posX, int posY)
    {
        constexpr int OFFSET_X = SCREEN_POSITION_X(2.0f);//SCREEN_POSITION_X(50.0f);
        constexpr int OFFSET_Y = -(SCREEN_POSITION_Y(9.0f)) + 3;

        if (posX != NO_VALUE)
        {
            g_StringPosition.x = posX;// - OFFSET_X;
            //g_StringPositionX1 = (s16)(posX - OFFSET_X);
        }

        if (posY != NO_VALUE)
        {
            g_StringPosition.y = posY;// - OFFSET_Y;
        }

        g_Strings2dLayerIdx = 6;
    }

    void Gfx_Strings2dLayerIdxSet(s32 idx) // 0x8004A8C0
    {
        g_Strings2dLayerIdx = idx;
    }

    void Gfx_StringsReset2dLayerIdx() // 0x8004A8CC
    {
        g_Strings2dLayerIdx = 6;
    }

    void Gfx_StringColorSet(e_StringColorId colorId)
    {
        g_StringColorId = colorId;
    }

    void DrawMsg(const std::string& msg, const Vector2& pos, int glyphCount)
    {
        struct State
        {
            Vector2i        Offset     = Vector2i::Zero;
            e_StringColorId ColorId    = StringColorId_White;
            int             StyleFlags = (int)TextStyleFlags::Gradient;
            AlignMode       AlignMd    = AlignMode::BottomLeft;
        };

        auto state = State{};

        auto nodes = ParseMsg(msg);
        for (const auto& node : nodes)
        {
            // Skip invalid node.
            if (node.Value.empty())
            {
                continue;
            }

            if (node.Type == NodeType::Text)
            {
                // Draw text string.
                int   drawnGlyphCount = GetCodePoints(node.Value).size();
                auto  str             = node.Value.substr(0, std::min(drawnGlyphCount, glyphCount - 1));
                float width           = Gfx_StringDraw(str, 400,
                                                       state.StyleFlags & (int)TextStyleFlags::HalfHeight,
                                                       state.AlignMd); // @todo Integrate width.

                // Stop drawing if length exceeded.
                glyphCount -= drawnGlyphCount;
                if (glyphCount <= 0)
                {
                    return;
                }
            }
            else if (node.Type == NodeType::Command)
            {
                char code = node.Value[0];
                int  arg  = node.Value[1];

                switch (code)
                {
                    case MAP_MSG_CODE_COLOR:
                    {
                        state.ColorId = (e_StringColorId)arg;
                        break;
                    }
                    case MAP_MSG_CODE_END:
                    {
                        // @todo
                        break;
                    }
                    case MAP_MSG_CODE_HALF_HEIGHT:
                    {
                        state.StyleFlags &= (int)TextStyleFlags::HalfHeight;
                        break;
                    }
                    case MAP_MSG_CODE_JUMP:
                    {
                        // @todo What do these values mean?
                        if (arg == 2)
                        {
                            g_MapMsg_AudioLoadBlock = 3;
                        }
                        break;
                    }
                    case MAP_MSG_CODE_LINE_POSITION:
                    {
                        // @todo
                        break;
                    }
                    case MAP_MSG_CODE_ALIGN_CENTER:
                    {
                        state.AlignMd = AlignMode::BottomLeft;
                        break;
                    }
                    case MAP_MSG_CODE_NEWLINE:
                    {
                        state.Offset.y += (state.StyleFlags &= (int)TextStyleFlags::HalfHeight) ? 16 : 8;
                        break;
                    }
                    case MAP_MSG_CODE_SELECT:
                    {
                        // @todo Returns `arg` as the result.
                        break;
                    }
                    case MAP_MSG_CODE_TAB:
                    {
                        state.Offset.x += 120;
                        break;
                    }
                    case MAP_MSG_CODE_PAGE:
                    {
                        // @todo Dynamic pages.
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
    }

    float Gfx_StringDraw(const std::string& str, int strLength, bool isHalfHeight, AlignMode alignMode)
    {
        constexpr float SCALE = RETRO_PIXEL_SCALE.y * 16.0f;

        const auto& options  = g_App.GetOptions();
        auto&       renderer = g_App.GetRenderer();

        // Submit text.
        auto fontName   = (options->TextQuality == TextQualityType::Retro) ? "RetroSerif" : "ModernSerif";
        auto pos        = GetGridAlignedScreenPercent(ConvertRetroScreenPixelsToPercent(g_StringPosition), (int)RETRO_SCREEN_SPACE_RES.y);
        int  styleFlags = (int)TextStyleFlags::Gradient | (isHalfHeight ? (int)TextStyleFlags::HalfHeight :
                                                                          (int)TextStyleFlags::None);
        auto text       = Text2d::CreateText2d(str, fontName,
                                               pos, 0.0f, SCALE, 1.0f,
                                               STRING_COLORS[g_StringColorId], styleFlags, true,
                                               6, alignMode);
        renderer.SubmitText2d(text);

        auto  fontScaleFactor  = SCREEN_SPACE_RES / (float)text.Font->GetPointSize();
        float heightScale      = (text.StyleFlags & (int)TextStyleFlags::HalfHeight) ? 0.5f : 1.0f;
        auto  textScale        = Vector2(1.0f, heightScale) * text.Scale;
        auto  textSize         = (Vector2(text.Shape.Width, (float)text.Font->GetPointSize()) * fontScaleFactor) * textScale;
        auto  aspectCorrection = GetScreenAspectCorrection(GLYPH_SCALE_MODE);

        // @todo Need to return width in retro resolution space (320x240).
        //float fontScaleFactor = SCREEN_SPACE_RES.x / (float)text.Font->GetPointSize();
        //auto  textWidth       = (text.Shape.Width * fontScaleFactor) * text.Scale;
        return textSize.x;// * aspectCorrection.x;
    }

    s32 Gfx_MapMsg_WidthsCompute(s32 mapMsgIdx) // 0x8004ACF4
    {
        s32 i;
        s32 j;
        s32 charCode;
        u8  msgCode;
        s32 msgArg;
        u8* mapMsg;

        g_MapMsg_WidthIdx  = 1;
        g_MapMsg_AudioLoadBlock = 0;

        // @todo Needs rewrite.

        /*for (i = (FONT_12X16_LINE_COUNT_MAX - 1); i >= 0; i--)
        {
            g_MapMsg_Widths[i] = 0;
        }

        mapMsg = g_MapOverlayHdr.mapMessages[mapMsgIdx];

        for (j = 0; j < FONT_12X16_LINE_COUNT_MAX; )
        {
            charCode = *mapMsg;

            switch (charCode)
            {
                case '\t':
                case '\n':
                case ' ':
                    mapMsg++;
                    break;

                case '_':
                    ++mapMsg;
                    g_MapMsg_Widths[g_MapMsg_WidthIdx - 1] += FONT_12X16_SPACE_SIZE;
                    break;

                case MAP_MSG_CODE_MARKER:
                    msgCode = *++mapMsg;
                    msgArg  = *++mapMsg - '0';

                    switch (msgCode)
                    {
                        case MAP_MSG_CODE_COLOR:
                        case MAP_MSG_CODE_SELECT:
                        case MAP_MSG_CODE_TAB:
                            break;

                        case MAP_MSG_CODE_NEWLINE:
                            j++;
                            g_MapMsg_WidthIdx++;
                            break;

                        case MAP_MSG_CODE_END:
                            j = FONT_12X16_LINE_COUNT_MAX;
                            break;

                        case MAP_MSG_CODE_LINE_POSITION:
                            g_PositionIdx = msgArg;
                            break;

                        case MAP_MSG_CODE_JUMP:
                            if (msgArg == 2)
                            {
                                g_MapMsg_AudioLoadBlock = 3;
                            }

                            while (msgArg != ' ' && msgArg != '\t')
                            {
                                msgArg = *++mapMsg;
                            }

                            break;

                        case MAP_MSG_CODE_HIGH_RES:
                            //g_SysWork.enableHighResGlyphs = true;
                            break;
                    }

                    mapMsg++;
                    break;

                case 0:
                    j = FONT_12X16_LINE_COUNT_MAX;
                    break;

                default:
                    // Convert literal `!` and `&` into `char`s mappable to representative atlas glyphs.
                    if (charCode == '!')
                    {
                        charCode = '\\';
                    }
                    else if (charCode == '&')
                    {
                        charCode = '^';
                    }

                    g_MapMsg_Widths[g_MapMsg_WidthIdx - 1] += FONT_12X16_GLYPH_WIDTHS[charCode - GLYPH_TABLE_ASCII_OFFSET];
                    mapMsg++;
                    break;
            }
        }*/
    }

    s32 Gfx_MapMsg_StringDraw(const std::string& mapMsg, s32 strLength, bool isHalfHeight) // 0x8004AF18
    {
        s32 glyphPosX;
        s32 glyphPosY;
        u32 temp_a0;
        s32 temp_a0_2;
        s32 i;
        s32 longestLineWidth;
        s32 lineIdx;
        s32 idx;
        s32 charWidth;

        int returnCode = MapMsgReturnCode_None;

        g_StringPosition.x = -(g_MapMsg_Widths[0] / 2);

        switch (g_PositionIdx)
        {
            case 0: // @unused
                g_StringPosition.y = -92;
                break;

            case 1: // @unused
                g_StringPosition.y = 76 - ((g_MapMsg_WidthIdx - 1) * FONT_12X16_GLYPH_SIZE_Y);
                break;

            case 2: // @unused
                g_StringPosition.y = -60;
                break;

            case 3: // @unused
                g_StringPosition.y = 44 - ((g_MapMsg_WidthIdx - 1) * FONT_12X16_GLYPH_SIZE_Y);
                break;

            case 4:
                g_StringPosition.y = ((FONT_12X16_LINE_COUNT_MAX - g_MapMsg_WidthIdx) * 8) - 76;
                break;
        }

        longestLineWidth = g_MapMsg_Widths[0];
        for (i = 0; i < g_MapMsg_WidthIdx; i++)
        {
            if (longestLineWidth < g_MapMsg_Widths[i])
            {
                longestLineWidth = g_MapMsg_Widths[i];
            }
        }

        g_StringPosition.x = -(longestLineWidth / 2);
        g_StringPositionX1 = g_StringPosition.x;
        glyphPosX          = g_StringPositionX1;
        glyphPosY          = g_StringPosition.y;

        auto alignMode = AlignMode::BottomLeft;

        // Process message nodes.
        auto nodes = ParseMsg(mapMsg);
        for (const auto& node : nodes)
        {
            // Skip invalid node.
            if (node.Value.empty())
            {
                continue;
            }

            if (node.Type == NodeType::Command)
            {
                char code    = node.Value[0];
                int  codeArg = code - '0';

                switch (code)
                {
                    case MAP_MSG_CODE_COLOR:
                    {
                        g_StringColorId = (e_StringColorId)codeArg;
                        break;
                    }
                    case MAP_MSG_CODE_DISPLAY_ALL:
                    {
                        strLength = MAP_MESSAGE_DISPLAY_ALL_LENGTH;
                        break;
                    }
                    case MAP_MSG_CODE_END:
                    {
                        returnCode  = NO_VALUE;
                        lineIdx = FONT_12X16_LINE_COUNT_MAX;
                        break;
                    }
                    case MAP_MSG_CODE_JUMP:
                    {
                        // Skip if another message already in progress.
                        if (g_SysWork.mapMsgTimer != NO_VALUE)
                        {
                            break;
                        }

                        // Parse time value.
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
                        break;
                    }
                    case MAP_MSG_CODE_ALIGN_CENTER:
                    {
                        returnCode = MapMsgReturnCode_AlignCenter;
                        glyphPosX  = -(g_MapMsg_Widths[lineIdx] / 2);
                        alignMode  = AlignMode::CenterBottom;
                        break;
                    }
                    case MAP_MSG_CODE_NEWLINE:
                    {
                        lineIdx++;

                        switch (returnCode)
                        {
                            case MapMsgReturnCode_AlignCenter:
                                glyphPosX = -(g_MapMsg_Widths[lineIdx] / 2);
                                break;

                            case MapMsgReturnCode_SetByT:
                                glyphPosX = g_StringPositionX1;
                                break;

                            default:
                                glyphPosX = -(longestLineWidth / 2);
                                break;
                        }

                        glyphPosY += FONT_12X16_GLYPH_SIZE_Y;
                        break;
                    }
                    case MAP_MSG_CODE_SELECT:
                    {
                        returnCode = codeArg;
                        lineIdx    = FONT_12X16_LINE_COUNT_MAX;
                        break;
                    }
                    case MAP_MSG_CODE_TAB:
                    {
                        returnCode         = MapMsgReturnCode_SetByT;
                        g_StringPositionX1 = -120;
                        glyphPosX          = -120;
                        break;
                    }
                    case MAP_MSG_CODE_ALIGN_RIGHT:
                    {
                        // @todo
                        break;
                    }
                    case MAP_MSG_CODE_PAGE:
                    {
                        // @todo
                        break;
                    }
                    default:
                    {
                        continue;
                    }
                }
            }
            else if (node.Type == NodeType::Text)
            {
                // Draw text string.
                int   charCount = GetCodePoints(node.Value).size();
                auto  text      = node.Value.substr(0, std::min(charCount, strLength - 1));
                float width     = Gfx_StringDraw(text, 400, isHalfHeight, alignMode); // @todo Integrate width.

                // Stop drawing if length exceeded.
                strLength -= charCount;
                if (strLength <= 0)
                {
                    return returnCode;
                }
            }
        }

        // @todo Apply offset.
        g_StringPosition = Vector2i(glyphPosX, glyphPosY);
        return returnCode;
    }

    void Gfx_MapMsg_DefaultStringInfoSet() // 0x8004B684
    {
        g_MapMsg_WidthIdx             = 1;
        g_PositionIdx                 = 1;
        g_StringPositionX1            = SCREEN_POSITION_X(-37.5f);
        g_StringColorId               = StringColorId_White;
    }

    void Gfx_StringDrawInt(s32 widthMin, s32 val) // 0x8004B9F8
    {
        Gfx_StringDraw(std::to_string(val), 5);
    }
}
