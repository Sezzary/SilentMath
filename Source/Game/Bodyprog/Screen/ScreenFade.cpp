#include "Framework.h"
#include "Psx.h"
#include "Game/Bodyprog/Screen/ScreenDraw.h"

#include "Game/Bodyprog/Bodyprog.h"

#include "Game/Bodyprog/Screen/ScreenData.h"
#include "Game/Main/FsQueue.h"

namespace Silent::Game
{
    q19_12 g_ScreenFadeTimestep;
    s32    g_Screen_FadeStatus; // @todo Temporary placement. Not migrated yet, don't know where to put it.
    q19_12 g_ScreenFadeProgress = Q12(0.0f);

    static q19_12 g_PrevScreenFadeProgress;

    static void Screen_FadeDrawModeSet() // 0x800325A4
    {
        if (IS_SCREEN_FADE_WHITE(g_Screen_FadeStatus))
        {
            //SetDrawMode(drMode, 0, 1, 32, NULL);
        }
        else
        {
            //SetDrawMode(drMode, 0, 1, 64, NULL);
        }
    }

    q19_12 Screen_FadeInProgressGet(void) // 0x800325F8
    {
        return Q12(1.0f) - g_PrevScreenFadeProgress;
    }

    void Screen_FadeUpdate(void) // 0x8003260C
    {
        s32 queueLength;
        s32 timestep;

        static auto dummyTile = TILE{};

        g_PrevScreenFadeProgress = g_ScreenFadeProgress;

        switch (g_Screen_FadeStatus)
        {
            case SCREEN_FADE_STATUS(ScreenFadeState_FadeOutStart, false):
            case SCREEN_FADE_STATUS(ScreenFadeState_FadeOutStart, true):
                g_ScreenFadeProgress = Q12(0.0f);
                g_Screen_FadeStatus++;

            case SCREEN_FADE_STATUS(ScreenFadeState_FadeOutSteps, false):
            case SCREEN_FADE_STATUS(ScreenFadeState_FadeOutSteps, true):
                Screen_FadeDrawModeSet();
                queueLength = Fs_QueueGetLength();

                if (g_ScreenFadeTimestep > Q12(0.0f))
                {
                    timestep = g_ScreenFadeTimestep;
                }
                else
                {
                    timestep = Q12(3.0f) / (queueLength + 1);
                }

                g_ScreenFadeProgress += Q12_MULT_PRECISE(timestep, g_DeltaTimeRaw);
                if (g_ScreenFadeProgress >= Q12_CLAMPED(1.0f))
                {
                    g_ScreenFadeProgress = Q12_CLAMPED(1.0f);
                    g_Screen_FadeStatus++;
                }
                break;

            case SCREEN_FADE_STATUS(ScreenFadeState_ResetTimestep, false):
            case SCREEN_FADE_STATUS(ScreenFadeState_ResetTimestep, true):
                g_ScreenFadeTimestep = Q12(0.0f);

            case SCREEN_FADE_STATUS(ScreenFadeState_FadeInStart, false):
            case SCREEN_FADE_STATUS(ScreenFadeState_FadeInStart, true):
                g_ScreenFadeProgress = Q12_CLAMPED(1.0f);
                g_Screen_FadeStatus++;

            case SCREEN_FADE_STATUS(ScreenFadeState_FadeOutComplete, false):
            case SCREEN_FADE_STATUS(ScreenFadeState_FadeOutComplete, true):
                Screen_FadeDrawModeSet();
                break;

            case SCREEN_FADE_STATUS(ScreenFadeState_FadeInSteps, false):
            case SCREEN_FADE_STATUS(ScreenFadeState_FadeInSteps, true):
                Screen_FadeDrawModeSet();

                if (g_ScreenFadeTimestep > Q12(0.0f))
                {
                    timestep = g_ScreenFadeTimestep;
                }
                else
                {
                    timestep = Q12(3.0f);
                }

                g_ScreenFadeProgress -= Q12_MULT_PRECISE(timestep, g_DeltaTimeRaw);

                if (g_ScreenFadeProgress <= Q12(0.0f))
                {
                    g_ScreenFadeProgress = Q12(0.0f);
                    ScreenFade_Reset();
                    return;
                }
                break;

            case SCREEN_FADE_STATUS(ScreenFadeState_Reset, false):
                g_ScreenFadeTimestep = Q12(0.0f);
                g_ScreenFadeProgress = Q12(0.0f);
                g_Screen_FadeStatus  = SCREEN_FADE_STATUS(ScreenFadeState_None, false);
                return;

            case SCREEN_FADE_STATUS(ScreenFadeState_None, false):
                return;
        }

        // Submit to renderer. Turn `return`s into `breaks`.
    }
}
