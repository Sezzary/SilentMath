#include "Framework.h"
#include "Psx.h"
#include "Game/Bodyprog/Sys/GameMain.h"

#include "Game/Bodyprog/Bodyprog.h"

#include "Application.h"
#include "Assets/AssetStreamer.h"
#include "Game/Bodyprog/MemCard.h"
#include "Game/Bodyprog/Demo.h"
#include "Game/Bodyprog/Screen/ScreenDraw.h"
#include "Game/Bodyprog/Sys/Joy.h"
#include "Game/Bodyprog/Text/TextDraw.h"
#include "Game/Main/FsQueue.h"
#include "Game/Screens/BKonami/BKonami.h"
#include "Game/Screens/Options/Options.h"
#include "Game/Screens/SaveLoad/SaveLoad.h"
#include "Renderer/Renderer.h"
//#include "bodyprog/sound_system.h"

using namespace Silent::Assets;

namespace Silent::Game
{
    s32 g_Demo_FrameCount = 0;
    s32 g_WarmBootTimer   = 0;

    static s32 g_PrevVBlanks = 0;

    // Audio task for `SD_Call` meant to load some VAB audio.
    u16 g_BaseVabAudiosTaskId[] =
    {
        160,
        162,
        0
    };

    void (*g_GameStateUpdateFuncs[])(void) =
    {
        GameState_Boot_Update,
        GameState_KonamiLogo_Update,
        GameState_KcetLogo_Update,
        GameState_MovieIntroFadeIn_Update,
        GameState_AutoLoadSavegame_Update,
        GameState_MovieIntroAlternate_Update,
        GameState_MovieIntro_Update,
        GameState_MainMenu_Update,
        GameState_LoadSavegameScreen_Update,
        GameState_MovieOpening_Update,
        nullptr,//GameState_LoadScreen_Update,
        nullptr,//GameState_InGame_Update,
        nullptr,//GameState_MapEvent_Update,
        GameState_ExitMovie_Update,
        nullptr,//GameState_ItemScreens_Update,
        nullptr,//GameState_MapScreen_Update,
        GameState_LoadSavegameScreen_Update,
        GameState_DebugMoviePlayer_Update,
        GameState_Options_Update,
        nullptr,//GameState_LoadStatusScreen_Update,
        nullptr,//GameState_LoadMapScreen_Update,
        nullptr,//GameState_Unk15_Update
    };

    void GameState_Boot_Update(void) // 0x80032D1C
    {
        e_GameState gameState;
        s32         vabAudioTaskId;

        const auto& assets   = g_App.GetAssets();
        auto&       renderer = g_App.GetRenderer();

        switch (g_GameWork.gameStateStep_598[0])
        {
            case 0:
                g_GameWork.background2dColor_58C.r = 0;
                g_GameWork.background2dColor_58C.g = 0;
                g_GameWork.background2dColor_58C.b = 0;

                Screen_Init(SCREEN_WIDTH, false);
                g_SysWork.counters_1C[1]              = 0;
                g_GameWork.gameStateStep_598[1] = 0;
                g_GameWork.gameStateStep_598[2] = 0;
                g_GameWork.gameStateStep_598[0]++;
                break;

            case 1:
                //if (!Sd_AudioStreamingCheck())
                {
                    vabAudioTaskId = g_BaseVabAudiosTaskId[g_GameWork.gameStateStep_598[1]];
                    if (vabAudioTaskId != 0)
                    {
                        //SD_Call(vabAudioTaskId);
                        g_GameWork.gameStateStep_598[1]++;
                    }
                    else
                    {
                        g_SysWork.counters_1C[1]              = 0;
                        g_GameWork.gameStateStep_598[1] = 0;
                        g_GameWork.gameStateStep_598[2] = 0;
                        g_GameWork.gameStateStep_598[0]++;
                    }
                }
                break;

            case 2:
                //Fs_QueueStartReadTim(FILE_1ST_FONT16_TIM, FS_BUFFER_1, &g_Font16AtlasImg);
                Fs_QueueStartReadTim(FILE_1ST_KONAMI_TIM, FS_BUFFER_1, &g_KonamiLogoImg);

                ScreenFade_Start(true, false, false);
                g_GameWork.gameStateStep_598[0]++;
                break;

            case 3:
                if (ScreenFade_IsFinished())
                {
                    // @todo Avoid blocking.
                    //Fs_QueueWaitForEmpty();
                    if (assets.IsBusy())
                    {
                        break;
                    }

                    gameState = g_GameWork.gameState_594;

                    g_SysWork.counters_1C[0] = 0;
                    g_SysWork.counters_1C[1] = 0;

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

        // Submit fullscreen sprite `1ST/2ZANKO_E.TIM.
        auto sprite = Sprite2d::CreateSprite2d("1ST/2ZANKO_E.TIM", Vector2::Zero, Vector2::One,
                                                SCREEN_SPACE_RES / 2.0f, DEG_TO_RAD(0.0f), 1.0f, Color::White,
                                                100, AlignMode::Center, ScaleMode::ShortEdge, BlendMode::Opaque);
        renderer.SubmitSprite2d(sprite);
        Debug::g_Work.BlendAlpha = std::clamp<float>(1.0f - FP_FLOAT(g_ScreenFadeProgress, Q8_SHIFT), 0, 1);
        //Screen_BackgroundImgDraw(&g_MainImg0);
    }
}
