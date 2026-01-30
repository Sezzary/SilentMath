#include "Framework.h"
#include "Game/Bodyprog/Screen/TextDraw.h"

#include "Application.h"
#include "Renderer/Renderer.h"
#include "Renderer/Common/Enums.h"
#include "Renderer/Common/Resources/Scene/Text2d.h"
#include "Renderer/Common/Utils.h"

using namespace Silent::Renderer;

namespace Silent::Game
{
    constexpr auto STRING_COLORS = std::array<Color, (int)e_StringColorId::Count>
    {
        Color::From8Bit(160, 128, 64),
        Color::From8Bit(32,  32,  32),
        Color::From8Bit(24,  128, 40),
        Color::From8Bit(8,   184, 96),
        Color::From8Bit(128, 0,   0),
        Color::From8Bit(100, 100, 100),
        Color::From8Bit(128, 128, 128)
    };

    static auto g_StringColorId = e_StringColorId::White;

    Vector2i g_StringPosition;
    int      g_StringPositionX1;

    #define SCREEN_WIDTH                   320
    #define SCREEN_HEIGHT                  240

    /** @brief Converts a floating-point X screen position in percent to a fixed-point X screen coodinate. */
    #define SCREEN_POSITION_X(percent) \
        (s32)(SCREEN_WIDTH * ((percent) / 100.0f))

    /** @brief Converts a floating-point Y screen position in percent to a fixed-point Y screen coodinate. */
    #define SCREEN_POSITION_Y(percent) \
        (s32)(SCREEN_HEIGHT * ((percent) / 100.0f))

    void Gfx_StringSetPosition(int posX, int posY)
    {
        #define OFFSET_X SCREEN_POSITION_X(2.0f)//SCREEN_POSITION_X(50.0f)
        #define OFFSET_Y -(SCREEN_POSITION_Y(9.0f)) + 3

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

        auto& renderer = g_App.GetRenderer();

        auto text = Text2d::CreateText2d(str, "RetroSerif",
                                         ConvertRetroScreenPixelsToPercent(g_StringPosition), 0.0f, SCALE, 1.0f,
                                         1.0f, TextStyle::Gradient, true,
                                         6, AlignMode::BottomLeft, ScaleMode::ShortEdge, BlendMode::Alpha);
        renderer.SubmitText2d(text);

        return true;
    }
}
