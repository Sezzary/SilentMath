#include "Framework.h"
#include "Psx.h"
#include "Game/Bodyprog/Text/TextDraw.h"

#include "Application.h"
#include "Game/Bodyprog/Events/MapMsgDisplay.h"
#include "Renderer/Renderer.h"
#include "Renderer/Common/Enums.h"
#include "Renderer/Common/Resources/Scene/Text2d.h"
#include "Renderer/Common/Utils.h"

using namespace Silent::Renderer;

namespace Silent::Game
{
    constexpr int SCREEN_WIDTH  = 320;
    constexpr int SCREEN_HEIGHT = 240;

    /** @brief Converts a floating-point X screen position in percent to a fixed-point X screen coodinate. */
    #define SCREEN_POSITION_X(percent) \
        (s32)(SCREEN_WIDTH * ((percent) / 100.0f))

    /** @brief Converts a floating-point Y screen position in percent to a fixed-point Y screen coodinate. */
    #define SCREEN_POSITION_Y(percent) \
        (s32)(SCREEN_HEIGHT * ((percent) / 100.0f))

    constexpr auto STRING_COLORS = std::array<Color, (int)StringColorId_Count>
    {
        Color::From8Bit(160, 128, 64),
        Color::From8Bit(32,  32,  32),
        Color::From8Bit(24,  128, 40),
        Color::From8Bit(8,   184, 96),
        Color::From8Bit(128, 0,   0),
        Color::From8Bit(100, 100, 100),
        Color::From8Bit(128, 128, 128)
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

    bool Gfx_StringDraw(const std::string& str, int strLength)
    {
        constexpr float SCALE = RETRO_PIXEL_SCALE.y * 16.0f;

        const auto& options  = g_App.GetOptions();
        auto&       renderer = g_App.GetRenderer();
        
        auto fontName = (options->TextQuality == TextQualityType::Original) ? "RetroSerif" : "SmoothSerif";
        auto text     = Text2d::CreateText2d(str, fontName,
                                             ConvertRetroScreenPixelsToPercent(g_StringPosition), 0.0f, SCALE, 1.0f,
                                             1.0f, TextStyle::Gradient, true,
                                             6, AlignMode::BottomLeft, ScaleMode::ShortEdge, BlendMode::Alpha);
        renderer.SubmitText2d(text);

        return true;
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

    s32 Gfx_MapMsg_StringDraw(char* mapMsg, s32 strLength) // 0x8004AF18
    {
        /*#define LINE_SPACE_SIZE 32
        #define CHARCODE_OFFSET '\''

        s32       glyphPosX;
        s32       glyphPosY;
        u32       temp_a0;
        s32       temp_a0_2;
        s32       digit;
        s32       i;
        s32       fractionDigits;
        s32       longestLineWidth;
        bool      isFraction;
        s32       lineIdx;
        u32       color;
        u8        codeTag;
        s32       codeArg;
        s32       result;
        s32       charCode;
        s32       idx;
        s32       charWidth;
        GsOT*     ot;
        PACKET*   packet;
        DR_TPAGE* tPage;
        POLY_FT4* glyphPoly;
        SPRT*     glyphSprt;

        packet = nullptr;
        result = 0;

        ot                  = (GsOT*)&g_OtTags0[g_ActiveBufferIdx][6];
        color               = STRING_COLORS[g_StringColorId];
        g_StringPosition.vx = -(g_MapMsg_Widths[0] >> 1);

        if (!g_SysWork.enableHighResGlyphs)
        {
            packet = GsOUT_PACKET_P;
        }

        switch ((u8)D_800C38B0.positionIdx_1)
        {
            case 0:
                g_StringPosition.vy = -92;
                break;

            case 1:
                g_StringPosition.vy = 76 - ((g_MapMsg_WidthIdx - 1) * FONT_12X16_GLYPH_SIZE_Y);
                break;

            case 2:
                g_StringPosition.vy = -60;
                break;

            case 3:
                g_StringPosition.vy = 44 - ((g_MapMsg_WidthIdx - 1) * FONT_12X16_GLYPH_SIZE_Y);
                break;

            case 4:
                g_StringPosition.vy = ((FONT_12X16_LINE_COUNT_MAX - g_MapMsg_WidthIdx) * 8) - 76;
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

        g_StringPosition.vx = -(longestLineWidth >> 1);
        g_StringPositionX1  = g_StringPosition.vx;
        glyphPosX           = g_StringPositionX1;
        glyphPosY           = g_StringPosition.vy;

        // Parse string.
        for (lineIdx = 0; lineIdx < FONT_12X16_LINE_COUNT_MAX;)
        {
            // Convert literal `!` and `&` into `char`s mappable to representative atlas glyphs.
            charCode = *mapMsg;
            if (charCode == '!')
            {
                charCode = '\\';
            }
            else if (charCode == '&')
            {
                charCode = '^';
            }

            // Process `char`.
            switch (charCode)
            {
                // Space.
                case '_':
                    glyphPosX += FONT_12X16_SPACE_SIZE;
                    mapMsg++;
                    break;

                // Ignore spaces and tabs.
                case ' ':
                case '\t':
                    mapMsg++;
                    break;

                case MAP_MSG_CODE_MARKER:
                    codeTag = *++mapMsg;
                    codeArg = *++mapMsg - '0';

                    switch (codeTag)
                    {
                        default:
                            break;

                        case MAP_MSG_CODE_NEWLINE:
                            lineIdx++;

                            switch (result)
                            {
                                case MapMsgCode_AlignCenter:
                                    glyphPosX = -(g_MapMsg_Widths[lineIdx] >> 1);
                                    break;

                                case MapMsgCode_SetByT:
                                    glyphPosX = g_StringPositionX1;
                                    break;

                                default:
                                    glyphPosX = -(longestLineWidth >> 1);
                                    break;
                            }

                            glyphPosY += FONT_12X16_GLYPH_SIZE_Y;
                            break;

                        case MAP_MSG_CODE_JUMP:
                            fractionDigits = 0;
                            isFraction     = 0;
                            digit          = 0;

                            // Parse time value.
                            if (g_SysWork.mapMsgTimer == NO_VALUE)
                            {
                                s32 c;

                                mapMsg                  = mapMsg + 2;
                                c                       = *mapMsg;
                                g_MapMsg_AudioLoadBlock = codeArg + 1;

                                while (c != ')')
                                {
                                    if (c == '.')
                                    {
                                        isFraction = 1;
                                    }
                                    else
                                    {
                                        if (isFraction != 0)
                                        {
                                            fractionDigits++;
                                        }

                                        digit *= 10;
                                        digit -= '0' - c;
                                    }

                                    mapMsg++;
                                    c = *mapMsg;
                                }

                                digit = digit << 12;
                                for (i = 0; i < fractionDigits; i++)
                                {
                                    digit = digit / 10;
                                }

                                g_SysWork.mapMsgTimer = digit;
                                mapMsg                     = mapMsg + 1;
                            }
                            else
                            {
                                while (codeArg != ' ' && codeArg != '\t')
                                {
                                    codeArg = *++mapMsg;
                                }
                            }
                            break;

                        case MAP_MSG_CODE_MIDDLE:
                            result    = MapMsgCode_AlignCenter;
                            glyphPosX = -(g_MapMsg_Widths[lineIdx] >> 1);
                            break;

                        case MAP_MSG_CODE_TAB:
                            result             = MapMsgCode_SetByT;
                            g_StringPositionX1 = -120;
                            glyphPosX          = -120;
                            break;

                        case MAP_MSG_CODE_COLOR:
                            color           = STRING_COLORS[codeArg];
                            g_StringColorId = codeArg;
                            break;

                        case MAP_MSG_CODE_DISPLAY_ALL:
                            strLength = MAP_MESSAGE_DISPLAY_ALL_LENGTH;
                            break;

                        case MAP_MSG_CODE_END:
                            result  = NO_VALUE;
                            lineIdx = FONT_12X16_LINE_COUNT_MAX;
                            break;

                        case MAP_MSG_CODE_SELECT:
                            result  = codeArg;
                            lineIdx = FONT_12X16_LINE_COUNT_MAX;
                            break;

                        case MAP_MSG_CODE_HIGH_RES:
                            g_SysWork.enableHighResGlyphs = true;
                            break;
                }

                mapMsg++;
                break;

            // Terminator.
            case '\0':
                result  = 1;
                lineIdx = FONT_12X16_LINE_COUNT_MAX;
                break;

            // Draw glyph sprite.
            default:
                strLength--;

                if (g_SysWork.enableHighResGlyphs)
                {
                    glyphPoly = (POLY_FT4*)GsOUT_PACKET_P;

                    idx       = charCode - CHARCODE_OFFSET;
                    charWidth = FONT_12X16_GLYPH_WIDTHS[charCode - CHARCODE_OFFSET];

                    setPolyFT4(glyphPoly);
                    setRGB0(glyphPoly, (s8)color, (s8)(color >> 8), (s8)(color >> 16));
                    setXY4(glyphPoly,
                        glyphPosX,                             glyphPosY * 2,
                        glyphPosX,                             (glyphPosY * 2) + 30,
                        glyphPosX + FONT_12X16_GLYPH_SIZE_X, glyphPosY * 2,
                        glyphPosX + FONT_12X16_GLYPH_SIZE_X, (glyphPosY * 2) + 30);

                    glyphPosX += charWidth;

                    temp_a0 = (idx % FONT_12X16_ATLAS_COLUMN_COUNT) * FONT_12X16_GLYPH_SIZE_X;

                    *((u32*)&glyphPoly->u0) = temp_a0 + 0xF000 + (0x7FD3 << 16);                                                     // `u0`, `v0`, `clut`.
                    *((u32*)&glyphPoly->u1) = temp_a0 + (((((idx / FONT_12X16_ATLAS_COLUMN_COUNT) & 0xF) | 0x10) << 16) | 0xFF00); // `u1`, `v1`, `page`.
                    *((u16*)&glyphPoly->u2) = temp_a0 - 0xFF4;
                    *((u16*)&glyphPoly->u3) = temp_a0 - 244;

                    addPrim(ot, glyphPoly);
                    GsOUT_PACKET_P = (PACKET*)glyphPoly + sizeof(POLY_FT4);
                }
                else
                {
                    temp_a0_2 = (u16)glyphPosX;

                    glyphSprt              = (SPRT*)packet;
                    *((u32*)&glyphSprt->w) = 0x10000C;

                    idx        = charCode - CHARCODE_OFFSET;
                    glyphPosX += FONT_12X16_GLYPH_WIDTHS[idx];

                    addPrimFast(ot, glyphSprt, 4);
                    *((u32*)&glyphSprt->r0)   = color;
                    *((u32*)(&glyphSprt->x0)) = temp_a0_2 + ((glyphPosY) << 16);
                    *((u32*)&glyphSprt->u0)   = (s32)(((idx % FONT_12X16_ATLAS_COLUMN_COUNT) * FONT_12X16_GLYPH_SIZE_X) + 0xF000 + (0x7FD3 << 16)); // `u0`, `v0`, `clut`.

                    packet += sizeof(SPRT);

                    tPage = (DR_TPAGE*)packet;
                    setDrawTPage(tPage, 0, 1, ((idx / FONT_12X16_ATLAS_COLUMN_COUNT) & 0xF) | 0x10);
                    addPrim(ot, tPage);

                    packet += sizeof(DR_TPAGE);
                }

                mapMsg++;

                if (strLength <= 0)
                {
                    if (!g_SysWork.enableHighResGlyphs)
                    {
                        GsOUT_PACKET_P = packet;
                    }

                    return result;
                }
            }
        }

        if (!g_SysWork.enableHighResGlyphs)
        {
            GsOUT_PACKET_P = packet;
        }

        *((u32*)&g_StringPosition) = (glyphPosX & 0xFFFF) + (glyphPosY << 16);
        return result;

        #undef LINE_SPACE_SIZE
        #undef CHARCODE_OFFSET*/
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
