#include "Framework.h"
#include "Psx.h"
#include "Game/Bodyprog/Text/TextDraw.h"

#include "Application.h"
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
    Vector2i    g_StringPosition;
    int         g_StringPositionX1;
    s_800C38B0  D_800C38B0;

    // @todo Not original placement.
    GsSPRITE D_800C38F8;

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

        //g_Strings2dLayerIdx = 6;
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
                D_800C38F8.x += GLYPH_SIZE_X;
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
                D_800C38F8.x -= GLYPH_SIZE_X;
            }

            val = quotient;
        }

        str--;
        *str = val + '0';

        if (isNegative)
        {
            str--;
            *str          = '-';
            D_800C38F8.x -= GLYPH_SIZE_X;
        }

        // Draw numeric string.
        Gfx_StringDraw(str, 5);
    }
}
