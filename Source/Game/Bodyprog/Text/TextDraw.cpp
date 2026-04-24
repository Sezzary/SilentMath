#include "Framework.h"
#include "Psx.h"
#include "Game/Bodyprog/Text/TextDraw.h"

#include "Game/Bodyprog/Bodyprog.h"

#include "Application.h"
#include "Game/Bodyprog/Events/MapMsgDisplay.h"
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
        Color::From8Bit(100, 100, 100),
        Color::From8Bit(128, 128, 128)
    };

    /** @brief Processed message node types. */
    enum class NodeType
    {
        Text,
        Command
    };

    /** @brief Processed message node. */
    struct MsgNode
    {
        NodeType    Type  = NodeType::Text;
        std::string Value = {};
    };

    static auto g_StringColorId = StringColorId_White;

    /** Text index 2D layer.
     * If modifying `Gfx_StringSetPosition`, when setting it to
     * a value lower than 6, text will not be affected by the fade effect.
     */
    static s32 g_Strings2dLayerIdx = 6;

    Vector2i    g_StringPosition;
    int         g_StringPositionX1;
    s_800C38B0  D_800C38B0;
    s32 g_MapMsg_WidthIdx;
    s32 g_MapMsg_Widths[12];
    GsSPRITE g_MapMsg_GlyphSprite;
    s16 D_800C391C;
    s32 D_800C3920;

    void Gfx_StringSetPosition(int posX, int posY)
    {
        constexpr int OFFSET_X = SCREEN_POSITION_X(2.0f);//SCREEN_POSITION_X(50.0f);
        constexpr int OFFSET_Y = -(SCREEN_POSITION_Y(9.0f)) + 3;

        if (posX != NO_VALUE)
        {
            g_StringPosition.x = posX - OFFSET_X;
            g_StringPositionX1 = (s16)(posX - OFFSET_X);
        }

        if (posY != NO_VALUE)
        {
            g_StringPosition.y = posY - OFFSET_Y;
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

    void Gfx_StringSetColor(e_StringColorId colorId)
    {
        g_StringColorId = colorId;
    }

    float Gfx_StringDraw(const std::string& str, int strLength)
    {
        constexpr float SCALE = RETRO_PIXEL_SCALE.y * 16.0f;

        const auto& options  = g_App.GetOptions();
        auto&       renderer = g_App.GetRenderer();

        // Submit text.
        auto fontName = (options->TextQuality == TextQualityType::Original) ? "RetroSerif" : "SmoothSerif";
        auto text     = Text2d::CreateText2d(str, fontName,
                                             ConvertRetroScreenPixelsToPercent(g_StringPosition), 0.0f, SCALE, 1.0f,
                                             STRING_COLORS[g_StringColorId], TextStyle::Gradient, true,
                                             6, AlignMode::BottomLeft, ScaleMode::ShortEdge, BlendMode::Alpha);
        renderer.SubmitText2d(text);

        // @todo Need to return width in retro resolution space (320x240).
        float fontScaleFactor = SCREEN_SPACE_RES.x / (float)text.Font->GetPointSize();
        auto  textWidth       = (text.Shape.Width * fontScaleFactor) * text.Scale;
        return textWidth;
    }

    s32 Gfx_MapMsg_CalculateWidths(s32 mapMsgIdx) // 0x8004ACF4
    {
        s32 i;
        s32 j;
        s32 charCode;
        u8  msgCode;
        s32 msgArg;
        u8* mapMsg;

        g_MapMsg_WidthIdx  = 1;
        g_MapMsg_AudioLoadBlock = 0;

        /*for (i = (FONT_12X16_LINE_COUNT_MAX - 1); i >= 0; i--)
        {
            g_MapMsg_Widths[i] = 0;
        }

        mapMsg = g_MapOverlayHeader.mapMessages_30[mapMsgIdx];

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
                            D_800C38B0.positionIdx_1 = msgArg;
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
                            g_SysWork.enableHighResGlyphs = true;
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

    s32 Gfx_MapMsg_StringDraw(const std::string& mapMsg, s32 strLength) // 0x8004AF18
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

        int result = 0;

        g_StringPosition.x = -(g_MapMsg_Widths[0] / 2);

        switch ((u8)D_800C38B0.positionIdx_1)
        {
            case 0:
                g_StringPosition.y = -92;
                break;

            case 1:
                g_StringPosition.y = 76 - ((g_MapMsg_WidthIdx - 1) * FONT_12X16_GLYPH_SIZE_Y);
                break;

            case 2:
                g_StringPosition.y = -60;
                break;

            case 3:
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

        // Process message nodes.
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
                        result  = NO_VALUE;
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
                    case MAP_MSG_CODE_MIDDLE:
                    {
                        result    = MapMsgCode_AlignCenter;
                        glyphPosX = -(g_MapMsg_Widths[lineIdx] / 2);
                        break;
                    }
                    case MAP_MSG_CODE_NEWLINE:
                    {
                        lineIdx++;

                        switch (result)
                        {
                            case MapMsgCode_AlignCenter:
                                glyphPosX = -(g_MapMsg_Widths[lineIdx] / 2);
                                break;

                            case MapMsgCode_SetByT:
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
                        result  = codeArg;
                        lineIdx = FONT_12X16_LINE_COUNT_MAX;
                        break;
                    }
                    case MAP_MSG_CODE_TAB:
                    {
                        result             = MapMsgCode_SetByT;
                        g_StringPositionX1 = -120;
                        glyphPosX          = -120;
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
                float width     = Gfx_StringDraw(text, 400); // @todo Integrate width.

                // Stop drawing if length exceeded.
                strLength -= charCount;
                if (strLength <= 0)
                {
                    return result;
                }
            }
        }

        // @todo Apply offset.
        g_StringPosition = Vector2i(glyphPosX, glyphPosY);
        return result;
    }

    void func_8004B658() // 0x8004B658
    {
        g_MapMsg_GlyphSprite.attribute = 64;
        g_MapMsg_GlyphSprite.cx        = 304;
        g_MapMsg_GlyphSprite.v         = 240;
        g_MapMsg_GlyphSprite.h         = 16;
    }

    void Gfx_MapMsg_DefaultStringInfoSet() // 0x8004B684
    {
        g_MapMsg_WidthIdx               = 1;
        D_800C38B0.field_0                   = 0;
        D_800C38B0.positionIdx_1             = 1;
        g_StringPositionX1                   = SCREEN_POSITION_X(-37.5f);
        g_StringColorId                      = StringColorId_White;
        //g_SysWork.enableHighResGlyphs = false;
    }

    void func_8004B6D4(s16 arg0, s16 arg1) // 0x8004B6D4
    {
        if (arg0 != NO_VALUE)
        {
            //g_MapMsg_GlyphSprite.x = arg0 + (-g_GameWork.gsScreenWidth / 2);
            D_800C391C   = g_MapMsg_GlyphSprite.x;
        }

        if (arg1 != NO_VALUE)
        {
            //g_MapMsg_GlyphSprite.y = arg1 + (-g_GameWork.gsScreenHeightx / 2);
        }
    }

    void func_8004B74C(s16 arg0) // 0x8004B74C
    {
        if (arg0 < 0 || arg0 >= 5)
        {
            //D_800C391E = 0;
            return;
        }

        //D_800C391E = arg0;
    }

    void func_8004B76C(char* str, bool useFixedWidth) // 0x8004B76C
    {
        /*#define GLYPH_SIZE_X       11
        #define GLYPH_SIZE_Y       12
        #define SPACE_SIZE         12
        #define LINE_SPACE_SIZE    16
        #define ATLAS_COLUMN_COUNT 21

        s32       tileRow;
        s32       glyphIdx;
        GsOT*     ot;
        GsSPRITE* glyphSprt;

        glyphSprt  = (GsSPRITE*)PSX_SCRATCH_ADDR(0x30);
        *glyphSprt = g_MapMsg_GlyphSprite;
        ot         = &g_OrderingTable2[g_ActiveBufferIdx];

        // Parse string.
        while (*str != '\0')
        {
            switch (*str)
            {
                // Draw glyph sprite.
                default:
                    glyphIdx     = *str - GLYPH_TABLE_ASCII_OFFSET;
                    tileRow      = glyphIdx / ATLAS_COLUMN_COUNT;
                    glyphSprt->u = (glyphIdx % ATLAS_COLUMN_COUNT) * GLYPH_SIZE_Y;

                    if (useFixedWidth)
                    {
                        glyphSprt->w = GLYPH_SIZE_X;
                    }
                    else
                    {
                        glyphSprt->w = FONT_12X16_GLYPH_WIDTHS[glyphIdx];
                    }

                    glyphSprt->tpage = (tileRow & 0xF) | 0x10;
                    glyphSprt->cx    = 304;
                    glyphSprt->cy    = D_800C391E + 506;

                    GsSortFastSprite(glyphSprt, ot, 4);

                    glyphSprt->x += glyphSprt->w;
                    break;

                // Space.
                case ' ':
                case '\t':
                    glyphSprt->x += SPACE_SIZE;
                    break;

                // Backspace.
                case '~':
                case '\b':
                    glyphSprt->x -= SPACE_SIZE;
                    break;

                // Newline.
                case '\n':
                    glyphSprt->x  = D_800C391C;
                    glyphSprt->y += LINE_SPACE_SIZE;
                    break;

                // Carriage return.
                case '\r':
                    glyphSprt->x  = D_800C391C;
                    glyphSprt->y -= LINE_SPACE_SIZE;
                    break;
            }

            str++;
        }

        g_MapMsg_GlyphSprite = *glyphSprt;

        #undef GLYPH_SIZE_X
        #undef GLYPH_SIZE_Y
        #undef SPACE_SIZE
        #undef LINE_SPACE_SIZE
        #undef ATLAS_COLUMN_COUNT*/
    }

    void Gfx_StringDrawInt(s32 widthMin, s32 val) // 0x8004B9F8
    {
        constexpr int GLYPH_SIZE_X       = 11;
        constexpr int ATLAS_COLUMN_COUNT = 10;

        s32   quotient;
        s32   isNegative;
        s32   i;
        char* str;
        char  buffer[47];

        if (widthMin > 0)
        {
            for (i = 0; i < (widthMin - 1); i++)
            {
                g_MapMsg_GlyphSprite.x += GLYPH_SIZE_X;
            }
        }

        str  = buffer;
        *str = 0;

        if (val < 0)
        {
            isNegative = true;
            val        = -val;
        }
        else
        {
            isNegative = false;
        }

        // Wrap atlas row?
        while (val >= ATLAS_COLUMN_COUNT)
        {
            str--;
            quotient = (val / ATLAS_COLUMN_COUNT) >> 32;
            *str     = (val - (quotient * ATLAS_COLUMN_COUNT)) + '0';

            if (widthMin > 0)
            {
                g_MapMsg_GlyphSprite.x -= GLYPH_SIZE_X;
            }

            val = quotient;
        }

        str--;
        *str = val + '0';

        if (isNegative)
        {
            str--;
            *str          = '-';
            g_MapMsg_GlyphSprite.x -= GLYPH_SIZE_X;
        }

        // Draw numeric string.
        Gfx_StringDraw(str, 5);
    }
}
