#include "Framework.h"
#include "Psx.h"
#include "Game/Screens/BKonami/BKonami.h"

#include "Game/Bodyprog/Bodyprog.h"

#include "Application.h"
#include "Game/Bodyprog/Demo.h"
#include "Game/Bodyprog/MemCard.h"
#include "Game/Bodyprog/Screen/BackgroundDraw.h"
#include "Game/Bodyprog/Screen/ScreenDraw.h"
#include "Game/Bodyprog/Screen/ScreenFade.h"
#include "Game/Bodyprog/Sys/FsScreens.h"
#include "Game/Bodyprog/Sys/Joy.h"
#include "Game/Main/FsQueue.h"
#include "Game/Screens/Stream/Stream.h"
#include "Input/Input.h"
#include "Renderer/Renderer.h"

using namespace Silent::Input;
using namespace Silent::Renderer;

namespace Silent::Game
{
    void GameState_KonamiLogo_Update() // 0x800C95AC
    {
        const auto& input = g_App.GetInput();

        //while (g_GameWork.gameState == GameState_KonamiLogo)
        switch (g_GameWork.gameStateSteps[0])
        {
            case KonamiLogoStateStep_Init:
                Screen_Init(SCREEN_WIDTH * 2, true);

                ScreenFade_Start(true, true, false);
                g_ScreenFadeTimestep = Q12(0.2f);

                // Load `1ST/KONAMI2.TIM` (Konami logo).
                Fs_QueueStartReadTim(FILE_1ST_KONAMI2_TIM, FS_BUFFER_1, &g_KcetLogoImg);

                //WorldGfx_HarryCharaLoad();
                //GameFs_BgItemLoad();
                //Map_EffectTexturesLoad(NO_VALUE);

                // Start loading `ANIM/HB_BASE.ANM` (base Harry animations).
                Fs_QueueStartRead(FILE_ANIM_HB_BASE_ANM, FS_BUFFER_0);

                g_GameWork.gameStateSteps[0]++;
                break;

            case KonamiLogoStateStep_WaitForFade:
                if (ScreenFade_IsNone())
                {
                    g_GameWork.gameStateSteps[0] = KonamiLogoStateStep_LogoDelay;
                }
                break;

            case KonamiLogoStateStep_LogoDelay:
                if (g_Controller0->btnsHeld_C != 0 || g_SysWork.counters_1C[0] > 180)
                {
                    ScreenFade_Start(false, false, false);
                    g_ScreenFadeTimestep         = Q12(0.2f);
                    g_GameWork.gameStateSteps[0] = KonamiLogoStateStep_FinishAfterFade;
                }
                break;

            case KonamiLogoStateStep_FinishAfterFade:
                if (ScreenFade_IsFinished())
                {
                    Fs_QueueWaitForEmpty();
                    Game_StateSetNext(GameState_KcetLogo);
                }
                break;
        }

        // @todo Original code depended on blocking logic, but the new way continues the game loop.
        // Need to do some light refactoring here for logos to show properly.
        //if (g_GameWork.gameState != GameState_KonamiLogo)
        {
            BootScreen_KonamiScreenDraw();
            //Screen_FadeUpdate();
            Fs_QueueUpdate();
            //MemCard_Update();
            //func_80033548();
            //VSync(SyncMode_Wait);
            //GsSwapDispBuff();
            //GsDrawOt(&g_OrderingTable2[g_ActiveBufferIdx]);

            //g_ActiveBufferIdx = GsGetActiveBuff();
            //GsOUT_PACKET_P   = (PACKET*)(TEMP_MEMORY_ADDR + (g_ActiveBufferIdx << 15));
            //GsClearOt(0, 0, &g_OrderingTable0[g_ActiveBufferIdx]);
            //GsClearOt(0, 0, &g_OrderingTable2[g_ActiveBufferIdx]);
        }

        if (input.GetAction(In::Enter).IsClicked())
        {
            Game_StateSetNext(GameState_KcetLogo);
        }
    }

    s32 GameState_KcetLogo_MemCardCheck() // 0x800C9874
    {
        // @stub
        return KcetLogoStateStep_HasSavegame;

        /*s32 saveEntryTypeSlot1;
        s32 saveEntryTypeSlot_2;

        // Memory cards not ready yet, rerun this on next frame.
        if (func_80033548() == false)
        {
            return KcetLogoStateStep_CheckMemCards;
        }

        g_MemCard_ActiveMemCardSlotSaves = (s_SaveScreenElement*)SAVEGAME_ENTRY_BUFFER_0;
        saveEntryTypeSlot1                = g_MemCard_ActiveMemCardSlotSaves->type_4;

        g_MemCard_ActiveMemCardSlotSaves = (s_SaveScreenElement*)SAVEGAME_ENTRY_BUFFER_1;
        saveEntryTypeSlot_2                = g_MemCard_ActiveMemCardSlotSaves->type_4;

        // No memory cards.
        if (saveEntryTypeSlot1 == SavegameEntryType_NoMemCard && saveEntryTypeSlot_2 == SavegameEntryType_NoMemCard)
        {
            return KcetLogoStateStep_NoMemCard;
        }

        // No free space on any card.
        if ((saveEntryTypeSlot1 == SavegameEntryType_OutOfBlocks && (saveEntryTypeSlot_2 == SavegameEntryType_OutOfBlocks || saveEntryTypeSlot_2 == SavegameEntryType_NoMemCard)) ||
            (saveEntryTypeSlot1 == SavegameEntryType_NoMemCard && saveEntryTypeSlot_2 == SavegameEntryType_OutOfBlocks))
        {
            return KcetLogoStateStep_NoMemCardFreeSpace;
        }

        if (saveEntryTypeSlot1 == SavegameEntryType_Save || saveEntryTypeSlot_2 == SavegameEntryType_Save)
        {
            g_MemCard_ActiveMemCardSlotSaves = MemCard_ActiveMemCardSlotGet(g_SelectedSaveSlotIdx);
            g_MemCard_ActiveMemCardSlotSaves = &g_MemCard_ActiveMemCardSlotSaves[g_SlotElementSelectedIdx[g_SelectedSaveSlotIdx]];

            g_SelectedDeviceId            = g_MemCard_ActiveMemCardSlotSaves->deviceId_5;
            g_SelectedFileIdx             = g_MemCard_ActiveMemCardSlotSaves->fileIdx_6;
            g_Savegame_SelectedElementIdx = g_MemCard_ActiveMemCardSlotSaves->elementIdx_7;

            return KcetLogoStateStep_HasSavegame;
        }

        return KcetLogoStateStep_NoSaveGame;*/
    }

    void GameState_KcetLogo_Update() // 0x800C99A4
    {
        static e_GameState nextGameState = GameState_Init;

        //while (g_GameWork.gameState == GameState_KcetLogo)
        switch (g_GameWork.gameStateSteps[0])
        {
            case KcetLogoStateStep_Init:
                //Settings_RestoreDefaults();

                ScreenFade_Start(true, true, false);
                g_ScreenFadeTimestep = Q12(0.2f);

                //GameFs_BgEtcGfxLoad();
                //Fs_QueueStartRead(FILE_BG_HP_SAFE1_BIN, FS_BUFFER_5);
                //Fs_QueueStartRead(FILE_BG_S__SAFE2_BIN, FS_BUFFER_6);
                g_GameWork.gameStateSteps[0]++;
                break;

            case KcetLogoStateStep_CheckMemCards:
                if (ScreenFade_IsNone())
                {
                    s32 curTime;

                    Fs_QueueWaitForEmpty();

                    //while (g_GameWork.gameStateSteps[0] < KcetLogoStateStep_NoMemCard)
                    {
                        g_GameWork.gameStateSteps[0] = GameState_KcetLogo_MemCardCheck();
                        //MemCard_Update();
                        //VSync(SyncMode_Wait);
                    }
                }
                break;

            case KcetLogoStateStep_NoMemCard:
                Fs_QueueStartReadTim(FILE_1ST_NO_MCD_E_TIM, FS_BUFFER_1, &D_800A900C);
                nextGameState = GameState_MovieIntroFadeIn;

                g_GameWork.gameStateSteps[0] = KcetLogoStateStep_LogoDelay;
                g_SysWork.counters_1C[1]        = 0;
                g_GameWork.gameStateSteps[1] = 0;
                g_GameWork.gameStateSteps[2] = 0;
                break;

            case KcetLogoStateStep_NoMemCardFreeSpace:
                Fs_QueueStartReadTim(FILE_1ST_NO_BLK_E_TIM, FS_BUFFER_1, &D_800A900C);
                nextGameState = GameState_MovieIntroFadeIn;

                g_GameWork.gameStateSteps[0] = KcetLogoStateStep_LogoDelay;
                g_SysWork.counters_1C[1]        = 0;
                g_GameWork.gameStateSteps[1] = 0;
                g_GameWork.gameStateSteps[2] = 0;
                break;

            case KcetLogoStateStep_NoSaveGame:
                GameFs_TitleGfxSeek();
                nextGameState = GameState_MovieIntro;

                g_GameWork.gameStateSteps[0] = KcetLogoStateStep_LogoDelay;
                g_SysWork.counters_1C[1]        = 0;
                g_GameWork.gameStateSteps[1] = 0;
                g_GameWork.gameStateSteps[2] = 0;
                break;

            case KcetLogoStateStep_HasSavegame:
                while (g_GameWork.gameStateSteps[1] < 3)
                {
                    switch (g_GameWork.gameStateSteps[1])
                    {
                        case 0:
                            //MemCard_ProcessSet(MemCardProcess_Load_Game, g_SelectedDeviceId, 0, 0);
                            g_GameWork.gameStateSteps[2] = 0;
                            g_GameWork.gameStateSteps[1]++;

                        case 1:
                            //if (MemCard_LastMemCardResultGet() != MemCardResult_Success)
                            //{
                                g_GameWork.gameStateSteps[2] = 0;
                                g_GameWork.gameStateSteps[1]++;
                            //}
                            break;

                        case 2:
                            if (g_GameWorkConst->config.optAutoLoad_25)
                            {
                                Fs_QueueStartRead(FILE_VIN_SAVELOAD_BIN, FS_BUFFER_1);
                                Fs_QueueStartSeek(FILE_TIM_SAVELOAD_TIM);
                                nextGameState = GameState_AutoLoadSavegame;
                            }
                            else
                            {
                                GameFs_TitleGfxSeek();
                                nextGameState = GameState_MovieIntro;
                            }

                            g_GameWork.gameStateSteps[2] = 0;
                            g_GameWork.gameStateSteps[1]++;
                            break;
                    }

                    //func_80033548();
                    //MemCard_Update();
                }

                g_GameWork.gameStateSteps[0] = KcetLogoStateStep_LogoDelay;
                g_SysWork.counters_1C[1]        = 0;
                g_GameWork.gameStateSteps[1] = 0;
                g_GameWork.gameStateSteps[2] = 0;
                break;

            case KcetLogoStateStep_LogoDelay:
                if (g_Controller0->btnsHeld_C != 0 || g_SysWork.counters_1C[0] > 180)
                {
                    ScreenFade_Start(false, false, false);
                    g_ScreenFadeTimestep = Q12(0.2f);
                    g_GameWork.gameStateSteps[0]++;
                }
                break;

            case KcetLogoStateStep_FinishAfterFade:
                if (ScreenFade_IsFinished())
                {
                    //Settings_ScreenAndVolUpdate();
                    Screen_Init(SCREEN_WIDTH, false);

                    switch (nextGameState)
                    {
                        case GameState_AutoLoadSavegame:
                            Fs_QueueStartReadTim(FILE_TIM_SAVELOAD_TIM, FS_BUFFER_7, &g_ItemInspectionImg);
                            break;

                        case GameState_MovieIntroFadeIn:
                            break;

                        case GameState_MovieIntroAlternate:
                        default:
                            GameFs_TitleGfxLoad();
                            break;
                    }

                    Demo_SequenceAdvance(0);
                    Demo_DemoDataRead();
                    Fs_QueueWaitForEmpty();

                    g_SysWork.counters_1C[0] = 0;
                    g_SysWork.counters_1C[1] = 0;

                    g_GameWork.gameStateSteps[1] = 0;
                    g_GameWork.gameStateSteps[2] = 0;

                    SysWork_StateSetNext(SysState_Gameplay);

                    g_GameWork.gameStateSteps[0] = g_GameWork.gameState;
                    g_GameWork.gameState         = nextGameState;
                    g_GameWork.gameStatePrev     = (e_GameState)g_GameWork.gameStateSteps[0];
                    g_GameWork.gameStateSteps[0] = 0;
                }
                break;
        }

        BootScreen_KcetScreenDraw();
        //Screen_FadeUpdate();
        Fs_QueueUpdate();
        //MemCard_Update();
        //func_80033548();
        //VSync(SyncMode_Wait);
        //GsSwapDispBuff();
        //GsDrawOt(&g_OrderingTable2[g_ActiveBufferIdx]);

        //g_ActiveBufferIdx = GsGetActiveBuff();
        //GsOUT_PACKET_P   = (g_ActiveBufferIdx << 0xF) + (u32)TEMP_MEMORY_ADDR;
        //GsClearOt(0, 0, &g_OrderingTable0[g_ActiveBufferIdx]);
        //GsClearOt(0, 0, &g_OrderingTable2[g_ActiveBufferIdx]);
    }

    void BootScreen_KonamiScreenDraw() // 0x800C9FB8
    {
        auto& renderer = g_App.GetRenderer();

        // Submit fullscreen sprite `1ST/KONAMI.TIM`.
        auto sprite = Sprite2d::CreateSprite2d("1ST/KONAMI.TIM", Vector2::Zero, Vector2::One,
                                               SCREEN_SPACE_RES / 2.0f, DEG_TO_RAD(0.0f), 1.0f, Color::White,
                                               100, AlignMode::Center, ScaleMode::ShortEdge, BlendMode::Opaque);
        renderer.SubmitSprite2d(sprite);
        Debug::g_Work.BlendAlpha = std::clamp<float>(1.0f - Q8_TO_FLT(g_ScreenFadeProgress), 0, 1);
    }

    void BootScreen_KcetScreenDraw() // 0x800CA120
    {
        auto& renderer = g_App.GetRenderer();

        // Submit fullscreen sprite `1ST/KONAMI2.TIM`.
        auto sprite = Sprite2d::CreateSprite2d("1ST/KONAMI2.TIM", Vector2::Zero, Vector2::One,
                                               SCREEN_SPACE_RES / 2.0f, DEG_TO_RAD(0.0f), 1.0f, Color::White,
                                               100, AlignMode::Center, ScaleMode::ShortEdge, BlendMode::Opaque);
        renderer.SubmitSprite2d(sprite);
        Debug::g_Work.BlendAlpha = std::clamp<float>(1.0f - Q8_TO_FLT(g_ScreenFadeProgress), 0, 1);
    }
}
