#include "Framework.h"
#include "Psx.h"
#include "Game/Screens/Options/Brightness.h"

#include "Game/Bodyprog/Bodyprog.h"

#include "Game/Bodyprog/Screen/ScreenFade.h"
#include "Game/Screens/Options/Options.h"
#include "Game/Screens/Options/SelectionGraphics.h"

namespace Silent::Game
{
    void Options_BrightnessMenu_Control()
    {
        // @todo
        // Handle menu state.
        switch (g_GameWork.gameStateSteps[1])
        {
            case BrightnessMenuState_0:
                // Entry.
                g_GameWork.gameStateSteps[1] = BrightnessMenuState_1;
                g_GameWork.gameStateSteps[2] = 0;
                break;

            case BrightnessMenuState_1:
                // Set fade.
                ScreenFade_Start(true, true, false);
                g_GameWork.gameStateSteps[1] = BrightnessMenuState_2;
                g_GameWork.gameStateSteps[2] = 0;
                break;

            case BrightnessMenuState_2:
                // Set config.
                if (g_Controller0->buttonFlags.pulsed & ControllerFlag_LStickHighLeft)
                {
                    if (g_GameWork.config.brightness != 0)
                    {
                        g_GameWork.config.brightness--;
                        //Sd_SfxPlay(Sfx_Back, 0, Q8_CLAMPED(0.25f));
                    }
                }
                if (g_Controller0->buttonFlags.pulsed & ControllerFlag_LStickHighRight)
                {
                    if (g_GameWork.config.brightness < 7)
                    {
                        g_GameWork.config.brightness++;
                        //Sd_SfxPlay(Sfx_Back, 0, Q8_CLAMPED(0.25f));
                    }
                }

                // Fade screen and leave menu.
                if (g_Controller0->buttonFlags.clicked & (g_GameWork.config.controllerConfig.enter |
                                                    g_GameWork.config.controllerConfig.cancel))
                {
                    if (g_Controller0->buttonFlags.clicked & g_GameWork.config.controllerConfig.enter)
                    {
                        //Sd_SfxPlay(Sfx_Confirm, 0, Q8_CLAMPED(0.25f));
                    }
                    else
                    {
                        //Sd_SfxPlay(Sfx_Cancel, 0, Q8_CLAMPED(0.25f));
                    }

                    //ScreenFade_Start(true, false, false);
                    g_GameWork.gameStateSteps[1]++;
                    g_GameWork.gameStateSteps[2] = 0;
                }
                break;

            case BrightnessMenuState_Leave:
                // Switch to previous menu.
                // TODO: Odd check for `ScreenFade_IsFinished()`.
                if (g_Screen_FadeStatus & (1 << 2) && !(g_Screen_FadeStatus & (1 << 1)) && g_Screen_FadeStatus & (1 << 0))
                {
                    ScreenFade_Start(true, true, false);
                    g_GameWork.gameStateSteps[0]    = OptionsMenuState_LeaveBrightness;
                    g_SysWork.counters_1C[1]                 = 0;
                    g_GameWork.gameStateSteps[1]    = 0;
                    g_GameWork.gameStateSteps[2]    = 0;
                    g_GameWork.background2dColor.r = 0;
                    g_GameWork.background2dColor.g = 0;
                    g_GameWork.background2dColor.b = 0;
                }
                break;
        }

        // @todo
        // Draw graphics.
        if (g_GameWork.gameStatePrev == GameState_MainMenu)
        {
            //Screen_BackgroundImgDraw(&g_BrightnessScreenImg0);
        }
        else
        {
            //Screen_BackgroundImgDraw(&g_BrightnessScreenImg1);
        }

        //func_8003E5E8(g_GameWork.config.brightness);
        Options_BrightnessMenu_ArrowsDraw();
        Options_BrightnessMenu_ConfigDraw();
    }

    void Options_BrightnessMenu_ConfigDraw()
    {
        // @todo
        //Gfx_StringColorSet(StringColorId_White);
        //Gfx_StringPositionSet(SCREEN_WIDTH / 4, 190);
        //Gfx_StringDraw("LEVEL_________", 20);
        //Gfx_StringDrawInt(1, g_GameWork.config.brightness);
    }

    void Options_BrightnessMenu_ArrowsDraw()
    {
        static const auto FRONT_ARROWS = std::vector<s_Triangle2d>
        {
            { { 8, 84  }, { 16, 76 }, { 16, 92 } },
            { { 64, 84 }, { 56, 76 }, { 56, 92 } }
        };

        static const auto BORDER_ARROWS = std::vector<s_Triangle2d>
        {
            { { 7, 84  }, { 17, 74 }, { 17, 94 } },
            { { 65, 84 }, { 55, 74 }, { 55, 94 } }
        };

        // Determine UI movement direction.
        int dir      = 0;
        if (g_Controller0->buttonFlags.held & ControllerFlag_LStickHighLeft)
        {
            dir = 1;
        }
        else if (g_Controller0->buttonFlags.held & ControllerFlag_LStickHighRight)
        {
            dir = 2;
        }
        else
        {
            dir = 0;
        }

        // Draw flashing left/right arrows.
        for (int i = 0; i < FRONT_ARROWS.size(); i++)
        {
            Options_Selection_ArrowDraw(FRONT_ARROWS[i], true);
        }

        // Draw border to highlight flashing left/right arrow corresponding to direction of UI navigation.
        for (int i = dir - 1; i < dir; i++)
        {
            Options_Selection_ArrowDraw(BORDER_ARROWS[i], false);
        }
    }
}
