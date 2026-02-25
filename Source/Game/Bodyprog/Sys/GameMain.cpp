#include "Framework.h"
#include "Game/Bodyprog/Sys/GameMain.h"

#include "Game/Game.h"
#include "Game/Bodyprog/Bodyprog.h"

#include "Game/Main/FsQueue.h"

namespace Silent::Game
{
    s32 g_Demo_FrameCount = 0;
    s32 g_UnknownFrameCounter = 0;
    s32 g_PrevVBlanks = 0;

    // Audio task for `SD_Call` meant to load some VAB audio.
    u16 D_800A9774[] =
    {
        160,
        162,
        0
    };

    void (*g_GameStateUpdateFuncs[])(void) =
    {
        nullptr,//GameState_Boot_Update,
        nullptr,//GameState_KonamiLogo_Update,
        nullptr,//GameState_KcetLogo_Update,
        nullptr,//GameState_MovieIntroFadeIn_Update,
        nullptr,//GameState_AutoLoadSavegame_Update,
        nullptr,//GameState_MovieIntroAlternate_Update,
        nullptr,//GameState_MovieIntro_Update,
        nullptr,//GameState_MainMenu_Update,
        nullptr,//GameState_LoadSavegameScreen_Update,
        nullptr,//GameState_MovieOpening_Update,
        nullptr,//GameState_LoadScreen_Update,
        nullptr,//GameState_InGame_Update,
        nullptr,//GameState_MapEvent_Update,
        nullptr,//GameState_ExitMovie_Update,
        nullptr,//GameState_ItemScreens_Update,
        nullptr,//GameState_MapScreen_Update,
        nullptr,//GameState_LoadSavegameScreen_Update,
        nullptr,//GameState_DebugMoviePlayer_Update,
        nullptr,//GameState_Options_Update,
        nullptr,//GameState_LoadStatusScreen_Update,
        nullptr,//GameState_LoadMapScreen_Update,
        nullptr,//GameState_Unk15_Update
    };

    void GameState_Boot_Update(void) // 0x80032D1C
    {
        e_GameState gameState;
        s32         unkGameStateVar;

        switch (g_GameWork.gameStateStep_598[0])
        {
            case 0:
                g_GameWork.background2dColor_58C.r = 0;
                g_GameWork.background2dColor_58C.g = 0;
                g_GameWork.background2dColor_58C.b = 0;

                //Screen_Init(SCREEN_WIDTH, false);
                g_SysWork.timer_20              = 0;
                g_GameWork.gameStateStep_598[1] = 0;
                g_GameWork.gameStateStep_598[2] = 0;
                g_GameWork.gameStateStep_598[0]++;
                break;

            case 1:
                //if (!Sd_AudioStreamingCheck())
                {
                    unkGameStateVar = D_800A9774[g_GameWork.gameStateStep_598[1]];
                    if (unkGameStateVar != 0)
                    {
                        //SD_Call(unkGameStateVar);
                        g_GameWork.gameStateStep_598[1]++;
                    }
                    else
                    {
                        g_SysWork.timer_20              = 0;
                        g_GameWork.gameStateStep_598[1] = 0;
                        g_GameWork.gameStateStep_598[2] = 0;
                        g_GameWork.gameStateStep_598[0]++;
                    }
                }
                break;

            case 2:
                //Fs_QueueStartReadTim(FILE_1ST_FONT16_TIM, FS_BUFFER_1, &g_Font16AtlasImg);
                //Fs_QueueStartReadTim(FILE_1ST_KONAMI_TIM, FS_BUFFER_1, &g_KonamiLogoImg);

                //ScreenFade_Start(true, false, false);
                g_GameWork.gameStateStep_598[0]++;
                break;

            case 3:
                //if (ScreenFade_IsFinished())
                {
                    Fs_QueueWaitForEmpty();

                    gameState = g_GameWork.gameState_594;

                    g_SysWork.timer_1C = 0;
                    g_SysWork.timer_20 = 0;

                    g_GameWork.gameStateStep_598[1] = 0;
                    g_GameWork.gameStateStep_598[2] = 0;

                    SysWork_StateSetNext(SysState_Gameplay);

                    g_GameWork.gameStateStep_598[0] = gameState;
                    g_GameWork.gameState_594        = (e_GameState)((int)gameState + 1);
                    g_GameWork.gameStatePrev_590    = gameState;
                    g_GameWork.gameStateStep_598[0] = 0;
                }
                break;
        }

        //func_80033548();
        //Gfx_BackgroundSpriteDraw(&g_MainImg0);
        //func_80089090(1);
    }
}
