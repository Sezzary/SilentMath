#include "Framework.h"
#include "Psx.h"
#include "Game/Screens/Options/Options.h"

#include "Game/Bodyprog/Bodyprog.h"

#include "Application.h"
#include "Game/Main/FsQueue.h"
#include "Game/Screens/Options/MenuGraphics.h"
#include "Input/Input.h"

using namespace Silent::Input;

namespace Silent::Game
{
    constexpr int LINE_CURSOR_TIMER_MAX = 8;

    int  g_MainOptionsMenu_SelectedEntry      = 0;
    int  g_ExtraOptionsMenu_SelectedEntry     = 0;
    int  g_MainOptionsMenu_PrevSelectedEntry  = 0;
    int  g_ExtraOptionsMenu_PrevSelectedEntry = 0;
    bool g_ScreenPosMenu_InvertBackgroundFade = false;

    /** @brief Tracks movement time of the cursor highlight. */
    s32 g_Options_SelectionHighlightTimer = 0;

    /** @brief Number of options to show in the extra options screen. Shows extra unlockable settings if they are unlocked. */
    static s32 g_ExtraOptionsMenu_EntryCount = 0;

    static s32 g_ExtraOptionsMenu_SelectedBloodColorEntry = 0;
    static s32 g_ExtraOptionsMenu_BulletMultMax           = 0;

    void GameState_Options_Update()
    {
        if (g_GameWork.gameStatePrev == GameState_InGame)
        {
            //func_800363D0();
        }

        if (g_GameWork.gameStatePrev != GameState_MainMenu)
        {
            //Game_TimerUpdate();
        }

        // Handle options menu state.
        int unlockedOptFlags = 0;
        switch (g_GameWork.gameStateSteps[0])
        {
            case OptionsMenuState_EnterMainOptions:
                //DrawSync(SyncMode_Wait);

                if (g_GameWork.gameStatePrev != GameState_InGame)
                {
                    //VSync(SyncMode_Wait8);
                }

                g_GameWork.background2dColor.r = 0;
                g_GameWork.background2dColor.g = 0;
                g_GameWork.background2dColor.b = 0;

                ScreenFade_Start(false, true, false);
                //g_IntervalVBlanks   = 1;

                if (g_GameWork.gameStatePrev == GameState_InGame)
                {
                    //func_80037188();
                }

                g_MainOptionsMenu_SelectedEntry      = MainOptionsMenuEntry_Exit;
                g_MainOptionsMenu_PrevSelectedEntry  = 0;
                g_ExtraOptionsMenu_SelectedEntry     = 0;
                g_ExtraOptionsMenu_PrevSelectedEntry = 0;
                g_Options_SelectionHighlightTimer    = 0;
                g_ExtraOptionsMenu_BulletMultMax     = 1;
                unlockedOptFlags                     = g_GameWork.config.optExtraOptionsEnabled_27;
                
                // Set available bullet multiplier.
                for (int i = 0; i < 5; i++)
                {
                    if (unlockedOptFlags & (1 << i))
                    {
                        g_ExtraOptionsMenu_BulletMultMax++;
                    }
                }

                // Set selected blood color.
                switch (g_GameWork.config.optExtraBloodColor_24)
                {
                    case BloodColor_Normal:
                        //g_ExtraOptionsMenu_SelectedBloodColorEntry = BloodColorMenuEntry_Normal;
                        break;

                    case BloodColor_Green:
                        //g_ExtraOptionsMenu_SelectedBloodColorEntry = BloodColorMenuEntry_Green;
                        break;

                    case BloodColor_Violet:
                        //g_ExtraOptionsMenu_SelectedBloodColorEntry = BloodColorMenuEntry_Violet;
                        break;

                    case BloodColor_Black:
                        //g_ExtraOptionsMenu_SelectedBloodColorEntry = BloodColorMenuEntry_Black;
                        break;
                }

                //g_ExtraOptionsMenu_EntryCount   = (g_GameWork.config.optExtraOptionsEnabled_27) ? 8 : 6;
                g_GameWork.gameStateSteps[0] = OptionsMenuState_MainOptions;
                g_SysWork.counters_1C[1]              = 0;
                g_GameWork.gameStateSteps[1] = 0;
                g_GameWork.gameStateSteps[2] = 0;
                break;

            case OptionsMenuState_LeaveScreenPos:
            case OptionsMenuState_LeaveBrightness:
            case OptionsMenuState_LeaveController:
                g_GameWork.gameStateSteps[0] = OptionsMenuState_MainOptions;
                g_SysWork.counters_1C[1]              = 0;
                g_GameWork.gameStateSteps[1] = 0;
                g_GameWork.gameStateSteps[2] = 0;
                break;

            case OptionsMenuState_EnterScreenPos:
                if (ScreenFade_IsFinished())
                {
                    g_GameWork.gameStateSteps[0] = OptionsMenuState_ScreenPos;
                    g_SysWork.counters_1C[1]              = 0;
                    g_GameWork.gameStateSteps[1] = 0;
                    g_GameWork.gameStateSteps[2] = 0;
                }
                break;

            case OptionsMenuState_ScreenPos:
                //Options_ScreenPosMenu_Control();
                break;

            case OptionsMenuState_EnterBrightness:
                if (false)//(ScreenFade_IsFinished())
                {
                    Fs_QueueWaitForEmpty();

                    g_GameWork.gameStateSteps[0] = OptionsMenuState_Brightness;
                    g_GameWork.gameStateSteps[0] = OptionsMenuState_Brightness;
                    g_SysWork.counters_1C[1]              = 0;
                    g_GameWork.gameStateSteps[1] = 0;
                    g_GameWork.gameStateSteps[2] = 0;
                }
                break;

            case OptionsMenuState_Brightness:
                //Options_BrightnessMenu_Control();
                break;

            case OptionsMenuState_EnterController:
                // Switch to controller menu.
                if (ScreenFade_IsFinished())
                {
                    g_GameWork.gameStateSteps[0] = OptionsMenuState_Controller;
                    g_GameWork.gameStateSteps[0] = OptionsMenuState_Controller;
                    g_SysWork.counters_1C[1]              = 0;
                    g_GameWork.gameStateSteps[1] = 0;
                    g_GameWork.gameStateSteps[2] = 0;
                }
                break;

            case OptionsMenuState_Controller:
                //Options_ControllerMenu_Control();
                break;

            case OptionsMenuState_Leave:
                ScreenFade_Start(true, false, false);

                g_GameWork.gameStateSteps[0] = OptionsMenuState_LeaveMainOptions;
                g_SysWork.counters_1C[1]              = 0;
                g_GameWork.gameStateSteps[1] = 0;
                g_GameWork.gameStateSteps[2] = 0;
                break;

            case OptionsMenuState_LeaveMainOptions:
                if (ScreenFade_IsFinished())
                {
                    // TODO: Likely `Game_StateSetPrevious` inline, but `gameState`/`gameStatePrev` loads inside are switched?

                    auto prevGameState = g_GameWork.gameStatePrev;
                    auto gameState     = g_GameWork.gameState;

                    g_SysWork.counters_1C[0]              = 0;
                    g_SysWork.counters_1C[1]              = 0;
                    g_GameWork.gameStateSteps[1] = 0;
                    g_GameWork.gameStateSteps[2] = 0;

                    SysWork_StateSetNext(SysState_Gameplay);

                    g_GameWork.gameStateSteps[0] = gameState;
                    g_GameWork.gameState        = prevGameState;
                    g_GameWork.gameStatePrev    = gameState;
                    g_GameWork.gameStateSteps[0] = OptionsMenuState_EnterMainOptions;
                }
                break;

            case OptionsMenuState_EnterExtraOptions:
                if (false)//(ScreenFade_IsFinished())
                {
                    g_GameWork.gameStateSteps[0]   = OptionsMenuState_ExtraOptions;
                    g_SysWork.counters_1C[1]                = 0;
                    ScreenFade_Start(false, true, false);
                    g_GameWork.gameStateSteps[1]   = 0;
                    g_GameWork.gameStateSteps[2]   = 0;
                    g_Options_SelectionHighlightTimer = 0;
                }
                break;

            case OptionsMenuState_LeaveExtraOptions:
                if (ScreenFade_IsFinished())
                {
                    g_GameWork.gameStateSteps[0] = OptionsMenuState_EnterMainOptions;
                    g_SysWork.counters_1C[1]              = 0;
                    g_GameWork.gameStateSteps[1] = 0;
                    g_GameWork.gameStateSteps[2] = 0;
                    ScreenFade_Start(false, true, false);
                }
                break;
        }

        // Handle menu state.
        switch (g_GameWork.gameStateSteps[0])
        {
            case OptionsMenuState_MainOptions:
            case OptionsMenuState_Leave:
            case OptionsMenuState_LeaveMainOptions:
            case OptionsMenuState_EnterScreenPos:
            case OptionsMenuState_EnterBrightness:
            case OptionsMenuState_EnterController:
            case OptionsMenuState_EnterExtraOptions:
                Options_MainOptionsMenu_Control();
                break;

            case OptionsMenuState_ExtraOptions:
            case OptionsMenuState_LeaveExtraOptions:
                Options_ExtraOptionsMenu_Control();
                break;
        }
    }

    void Options_MainOptionsMenu_Control()
    {
        constexpr int SOUND_VOL_STEP = 8;

        const auto& input = g_App.GetInput();

        // Draw graphics.
        Options_MainOptionsMenu_EntryStringsDraw();
        Options_MainOptionsMenu_ConfigDraw();
        Options_MainOptionsMenu_SelectionHighlightDraw();
        Options_Menu_VignetteDraw();
        //Screen_BackgroundImgDraw(&g_ItemInspectionImg);
        Options_MainOptionsMenu_BgmVolumeBarDraw();
        Options_MainOptionsMenu_SfxVolumeBarDraw();

        if (g_GameWork.gameStateSteps[0] != OptionsMenuState_MainOptions)
        {
            return;
        }

        // Increment line move timer.
        if ((LINE_CURSOR_TIMER_MAX - 1) < g_Options_SelectionHighlightTimer)
        {
            g_Options_SelectionHighlightTimer = LINE_CURSOR_TIMER_MAX;
        }
        else
        {
            g_Options_SelectionHighlightTimer++;
        }

        if (g_Options_SelectionHighlightTimer != LINE_CURSOR_TIMER_MAX)
        {
            return;
        }

        g_MainOptionsMenu_PrevSelectedEntry = g_MainOptionsMenu_SelectedEntry;

        // Leave to gameplay (if options menu was accessed with `Option` input action).
        if (g_GameWork.gameStatePrev == GameState_InGame &&
            !input.GetAction(In::Enter).IsClicked() && input.GetAction(In::Option).IsClicked())
        {
            //Sd_PlaySfx(Sfx_MenuCancel, 0, 64);

            g_GameWork.gameStateSteps[0] = OptionsMenuState_Leave;
            g_SysWork.counters_1C[1]              = 0;
            g_GameWork.gameStateSteps[1] = 0;
            g_GameWork.gameStateSteps[2] = 0;
            return;
        }

        // @todo Accurate pulse delay.
        // Move selection cursor up/down.
        if (input.GetAction(In::Up).IsPulsed(0.2f, 0.4f))
        {
            //Sd_PlaySfx(Sfx_MenuMove, 0, 64);

            g_Options_SelectionHighlightTimer = 0;
            g_MainOptionsMenu_SelectedEntry   = (g_MainOptionsMenu_SelectedEntry + (MainOptionsMenuEntry_Count - 1)) % MainOptionsMenuEntry_Count;
        }
        if (input.GetAction(In::Down).IsPulsed(0.2f, 0.4f))
        {
            //Sd_PlaySfx(Sfx_MenuMove, 0, 64);

            g_Options_SelectionHighlightTimer = 0;
            g_MainOptionsMenu_SelectedEntry   = (g_MainOptionsMenu_SelectedEntry + 1) % MainOptionsMenuEntry_Count;
        }

        // Handle config change.
        int vol = 0;
        switch (g_MainOptionsMenu_SelectedEntry)
        {
            case MainOptionsMenuEntry_Exit:
                // Exit menu to gameplay.
                if (input.GetAction(In::Enter).IsClicked() || input.GetAction(In::Cancel).IsClicked())
                {
                    //Sd_PlaySfx(Sfx_MenuCancel, 0, 64);

                    g_GameWork.gameStateSteps[0] = OptionsMenuState_Leave;
                    g_SysWork.counters_1C[1]              = 0;
                    g_GameWork.gameStateSteps[1] = 0;
                    g_GameWork.gameStateSteps[2] = 0;
                }
                break;

            case MainOptionsMenuEntry_Controller:
                // Enter controller screen.
                if (input.GetAction(In::Enter).IsClicked())
                {
                    //Sd_PlaySfx(Sfx_MenuConfirm, 0, 64);
                    Fs_QueueStartReadTim(FILE_TIM_OPTION2_TIM, IMAGE_BUFFER_3, &g_ControllerButtonAtlasImg);

                    ScreenFade_Start(true, false, false);
                    g_GameWork.gameStateSteps[0] = OptionsMenuState_EnterController;
                    g_SysWork.counters_1C[1]              = 0;
                    g_GameWork.gameStateSteps[1] = 0;
                    g_GameWork.gameStateSteps[2] = 0;
                }
                break;

            case MainOptionsMenuEntry_ScreenPosition:
                // Enter screen position screen.
                if (input.GetAction(In::Enter).IsClicked())
                {
                    //Sd_PlaySfx(Sfx_MenuConfirm, 0, 64);

                    ScreenFade_Start(true, false, false);
                    g_GameWork.gameStateSteps[0] = OptionsMenuState_EnterScreenPos;
                    g_SysWork.counters_1C[1]              = 0;
                    g_GameWork.gameStateSteps[1] = 0;
                    g_GameWork.gameStateSteps[2] = 0;
                }
                break;

            case MainOptionsMenuEntry_Brightness:
                if (input.GetAction(In::Enter).IsClicked())
                {
                    //Sd_PlaySfx(Sfx_MenuConfirm, 0, 64);
                    if (g_GameWork.gameStatePrev == GameState_MainMenu)
                    {
                        Fs_QueueStartReadTim(FILE_TIM_OP_BRT_E_TIM, IMAGE_BUFFER_3, &g_BrightnessScreenImg0);
                    }
                    else
                    {
                        Fs_QueueStartReadTim(FILE_TIM_OP_BRT_E_TIM, IMAGE_BUFFER_3, &g_BrightnessScreenImg1);
                    }

                    ScreenFade_Start(true, false, false);
                    g_GameWork.gameStateSteps[0] = OptionsMenuState_EnterBrightness;
                    g_SysWork.counters_1C[1]              = 0;
                    g_GameWork.gameStateSteps[1] = 0;
                    g_GameWork.gameStateSteps[2] = 0;
                }
                break;

            case MainOptionsMenuEntry_Vibration:
                if (input.GetAction(In::Left).IsClicked() || input.GetAction(In::Right).IsClicked())
                {
                    //Sd_PlaySfx(Sfx_MenuMove, 0, 64);
                    g_GameWork.config.optVibrationEnabled_21 = !g_GameWork.config.optVibrationEnabled_21 << 7;
                }
                break;

            case MainOptionsMenuEntry_AutoLoad:
                if (input.GetAction(In::Left).IsClicked() || input.GetAction(In::Right).IsClicked())
                {
                    //Sd_PlaySfx(Sfx_MenuMove, 0, 64);
                    g_GameWork.config.optAutoLoad_25 = (s8)g_GameWork.config.optAutoLoad_25 == 0;
                }
                break;

            case MainOptionsMenuEntry_Sound:
                if (input.GetAction(In::Left).IsClicked() || input.GetAction(In::Right).IsClicked())
                {
                    //Sd_PlaySfx(Sfx_MenuMove, 0, 64);

                    // Set config.
                    /*int audioType                           = AudioMode_Stereo;
                    g_GameWork.config.optSoundType_1E = !g_GameWork.config.optSoundType_1E;
                    if (g_GameWork.config.optSoundType_1E)
                    {
                        audioType = AudioMode_Mono;
                    }
                    Sd_EngineCmd(audioType);*/
                }
                break;

            case MainOptionsMenuEntry_BgmVolume:
                vol = g_GameWork.config.optVolumeBgm_1F;

                /*if ((vol < OPT_SOUND_VOLUME_MAX && input.GetAction(In::Right).IsPulsed(0.2f, 0.4f)) ||
                    (vol > 0                    && input.GetAction(In::Left).IsPulsed(0.2f, 0.4f)))
                {
                    Sd_EngineCmd(Sfx_MenuMove);
                }
                if ((vol == OPT_SOUND_VOLUME_MAX && input.GetAction(In::Right).IsClicked()) ||
                    (vol == 0                    && input.GetAction(In::Left).IsClicked()))
                {
                    Sd_EngineCmd(Sfx_MenuError);
                }*/

                // Scroll left/right.
                if (input.GetAction(In::Left).IsPulsed(0.2f, 0.4f))
                {
                    vol = vol - SOUND_VOL_STEP;
                }
                if (input.GetAction(In::Right).IsPulsed(0.2f, 0.4f))
                {
                    vol = vol + SOUND_VOL_STEP;
                }

                // Set config.
                //vol = CLAMP(vol, 0, OPT_SOUND_VOLUME_MAX);
                //Sd_SetVolume(OPT_SOUND_VOLUME_MAX, vol, g_GameWork.config.optVolumeSe_20);
                g_GameWork.config.optVolumeBgm_1F = vol;
                break;

            case MainOptionsMenuEntry_SfxVolume:
                vol = g_GameWork.config.optVolumeSe_20;

                /*if ((vol < OPT_SOUND_VOLUME_MAX && input.GetAction(In::Right).IsPulsed(0.2f, 0.4f)) ||
                    (vol > 0                    && input.GetAction(In::Left).IsPulsed(0.2f, 0.4f)))
                {
                    Sd_EngineCmd(Sfx_MenuMove);
                }
                if ((vol == OPT_SOUND_VOLUME_MAX && input.GetAction(In::Right).IsClicked()) ||
                    (vol == 0                    && input.GetAction(In::Left).IsClicked()))
                {
                    Sd_EngineCmd(Sfx_MenuError);
                }*/

                if (input.GetAction(In::Left).IsPulsed(0.2f, 0.4f))
                {
                    vol = vol - SOUND_VOL_STEP;
                }
                if (input.GetAction(In::Right).IsPulsed(0.2f, 0.4f))
                {
                    vol = vol + SOUND_VOL_STEP;
                }

                /*vol = CLAMP(vol, 0, OPT_SOUND_VOLUME_MAX);

                Sd_SetVolume(OPT_SOUND_VOLUME_MAX, vol, g_GameWork.config.optVolumeSe_20);*/
                g_GameWork.config.optVolumeSe_20 = vol;
                break;

            default:
                break;
        }

        if (input.GetAction(In::StepLeft).IsClicked()            || input.GetAction(In::StepRight).IsClicked()            ||
            input.GetAction(In::GamepadShoulderLeft).IsClicked() || input.GetAction(In::GamepadShoulderRight).IsClicked() ||
            input.GetAction(In::GamepadTriggerLeft).IsClicked()  || input.GetAction(In::GamepadTriggerRight).IsClicked())
        {
            if (g_GameWork.gameStateSteps[0] == OptionsMenuState_EnterExtraOptions)
            {
                return;
            }

            //Sd_PlaySfx(Sfx_MenuConfirm, 0, 64);

            ScreenFade_Start(true, false, false);
            g_GameWork.gameStateSteps[0] = OptionsMenuState_EnterExtraOptions;
            g_SysWork.counters_1C[1]              = 0;
            g_GameWork.gameStateSteps[1] = 0;
            g_GameWork.gameStateSteps[2] = 0;
        }

        // Reset selection cursor.
        if (((g_GameWork.gameStateSteps[0] != OptionsMenuState_EnterExtraOptions &&
              g_MainOptionsMenu_SelectedEntry != MainOptionsMenuEntry_Exit) &&
            !input.GetAction(In::Enter).IsClicked()) &&
            input.GetAction(In::Cancel).IsClicked())
        {
            //Sd_PlaySfx(Sfx_MenuCancel, 0, 64);

            g_Options_SelectionHighlightTimer = 0;
            g_MainOptionsMenu_SelectedEntry   = MainOptionsMenuEntry_Exit;
        }
    }

    void Options_ExtraOptionsMenu_Control()
    {
/*
        //Options_ExtraOptionsMenu_EntryStringsDraw();
        //Options_ExtraOptionsMenu_ConfigDraw();
        //Options_ExtraOptionsMenu_SelectionHighlightDraw();
        //Options_Menu_VignetteDraw();
        //Screen_BackgroundImgDraw(&g_ItemInspectionImg);

        if (g_GameWork.gameStateSteps[0] != OptionsMenuState_ExtraOptions)
        {
            return;
        }

        // Increment line move timer.
        if ((LINE_CURSOR_TIMER_MAX - 1) < g_Options_SelectionHighlightTimer)
        {
            g_Options_SelectionHighlightTimer = LINE_CURSOR_TIMER_MAX;
        }
        else
        {
            g_Options_SelectionHighlightTimer++;
        }

        if (g_Options_SelectionHighlightTimer == LINE_CURSOR_TIMER_MAX)
        {
            g_ExtraOptionsMenu_PrevSelectedEntry = g_ExtraOptionsMenu_SelectedEntry;

            // Leave to gameplay (if options menu was accessed with `Option` input action).
            if (g_GameWork.gameStatePrev == GameState_InGame && 
                !(g_Controller0->btnsClicked_10 & g_GameWorkPtr->config.controllerConfig_0.enter) &&
                (g_Controller0->btnsClicked_10 & g_GameWorkPtr->config.controllerConfig_0.option))
            {
                //Sd_PlaySfx(Sfx_Cancel, 0, 64);

                g_GameWork.gameStateSteps[0] = OptionsMenuState_Leave;
                g_SysWork.counters_1C[1]              = 0;
                g_GameWork.gameStateSteps[1] = 0;
                g_GameWork.gameStateSteps[2] = 0;
                return;
            }

            // Move selection cursor up/down.
            if (g_Controller0->btnsPulsed_18 & ControllerFlag_LStickUp)
            {
                s32 var = 1;
                //Sd_PlaySfx(Sfx_Back, 0, 64);
                g_ExtraOptionsMenu_SelectedEntry  = ((g_ExtraOptionsMenu_SelectedEntry - var) + g_ExtraOptionsMenu_EntryCount) % g_ExtraOptionsMenu_EntryCount;
                g_Options_SelectionHighlightTimer = 0;
            }
            if (g_Controller0->btnsPulsed_18 & ControllerFlag_LStickDown)
            {
                Sd_PlaySfx(Sfx_Back, 0, 64);
                g_ExtraOptionsMenu_SelectedEntry++;
                g_ExtraOptionsMenu_SelectedEntry  = g_ExtraOptionsMenu_SelectedEntry % g_ExtraOptionsMenu_EntryCount;
                g_Options_SelectionHighlightTimer = 0;
            }

            // Handle config change.
            switch (g_ExtraOptionsMenu_SelectedEntry)
            {
                case ExtraOptionsMenuEntry_WeaponCtrl:
                    // Scroll left/right.
                    if (g_Controller0->btnsClicked_10 & (ControllerFlag_LStickRight | ControllerFlag_LStickLeft))
                    {
                        Sd_PlaySfx(Sfx_Back, 0, 64);
                        g_GameWork.config.optExtraWeaponCtrl_23 = !g_GameWork.config.optExtraWeaponCtrl_23;
                    }
                    break;

                case ExtraOptionsMenuEntry_Blood:
                    // Scroll left/right.
                    if (g_Controller0->btnsClicked_10 & ControllerFlag_LStickRight)
                    {
                        Sd_PlaySfx(Sfx_Back, 0, 64);
                        g_ExtraOptionsMenu_SelectedBloodColorEntry++;
                    }
                    if (g_Controller0->btnsClicked_10 & ControllerFlag_LStickLeft)
                    {
                        Sd_PlaySfx(Sfx_Back, 0, 64);
                        g_ExtraOptionsMenu_SelectedBloodColorEntry += 3;
                    }

                    // Set config.
                    g_ExtraOptionsMenu_SelectedBloodColorEntry = g_ExtraOptionsMenu_SelectedBloodColorEntry % BloodColorMenuEntry_Count;
                    switch (g_ExtraOptionsMenu_SelectedBloodColorEntry)
                    {
                        case BloodColorMenuEntry_Normal:
                            g_GameWork.config.optExtraBloodColor_24 = BloodColor_Normal;
                            break;

                        case BloodColorMenuEntry_Green:
                            g_GameWork.config.optExtraBloodColor_24 = BloodColor_Green;
                            break;

                        case BloodColorMenuEntry_Violet:
                            g_GameWork.config.optExtraBloodColor_24 = BloodColor_Violet;
                            break;

                        case BloodColorMenuEntry_Black:
                            g_GameWork.config.optExtraBloodColor_24 = BloodColor_Black;
                            break;
                    }
                    break;

                case ExtraOptionsMenuEntry_ViewCtrl:
                    // Scroll left/right.
                    if (g_Controller0->btnsClicked_10 & (ControllerFlag_LStickRight | ControllerFlag_LStickLeft))
                    {
                        Sd_PlaySfx(Sfx_Back, 0, 64);

                        // Set config.
                        g_GameWork.config.optExtraViewCtrl_28 = !g_GameWork.config.optExtraViewCtrl_28;
                    }
                    break;

                case ExtraOptionsMenuEntry_RetreatTurn:
                    // Scroll left/right.
                    if (g_Controller0->btnsClicked_10 & (ControllerFlag_LStickRight | ControllerFlag_LStickLeft))
                    {
                        Sd_PlaySfx(Sfx_Back, 0, 64);

                        // Set config.
                        g_GameWork.config.optExtraRetreatTurn_2A = (s8)g_GameWork.config.optExtraRetreatTurn_2A == 0;
                    }
                    break;

                case ExtraOptionsMenuEntry_MovementCtrl:
                    // Scroll left/right.
                    if (g_Controller0->btnsClicked_10 & (ControllerFlag_LStickRight | ControllerFlag_LStickLeft))
                    {
                        Sd_PlaySfx(Sfx_Back, 0, 64);

                        // Set config.
                        g_GameWork.config.optExtraWalkRunCtrl_2B = (s8)g_GameWork.config.optExtraWalkRunCtrl_2B == 0;
                    }
                    break;

                case ExtraOptionsMenuEntry_AutoAiming:
                    // Scroll left/right.
                    if (g_Controller0->btnsClicked_10 & (ControllerFlag_LStickRight | ControllerFlag_LStickLeft))
                    {
                        Sd_PlaySfx(Sfx_Back, 0, 64);

                        // Set config.
                        g_GameWork.config.optExtraAutoAiming_2C = (s8)g_GameWork.config.optExtraAutoAiming_2C == 0;
                    }
                    break;

                case ExtraOptionsMenuEntry_ViewMode:
                    // Scroll left/right.
                    if (g_Controller0->btnsClicked_10 & (ControllerFlag_LStickRight | ControllerFlag_LStickLeft))
                    {
                        Sd_PlaySfx(Sfx_Back, 0, 64);

                        // Set config.
                        g_GameWork.config.optExtraViewMode_29 = !g_GameWork.config.optExtraViewMode_29;
                    }
                    break;

                case ExtraOptionsMenuEntry_BulletMult:
                    // Scroll left/right.
                    if (g_Controller0->btnsClicked_10 & ControllerFlag_LStickRight)
                    {
                        Sd_PlaySfx(Sfx_Back, 0, 64);

                        // Set config.
                        g_GameWork.config.optExtraBulletAdjust_2D++;
                    }
                    if (g_Controller0->btnsClicked_10 & ControllerFlag_LStickLeft)
                    {
                        Sd_PlaySfx(Sfx_Back, 0, 64);

                        // Set config.
                        g_GameWork.config.optExtraBulletAdjust_2D = g_GameWork.config.optExtraBulletAdjust_2D + (g_ExtraOptionsMenu_BulletMultMax - 1);
                    }
                    g_GameWork.config.optExtraBulletAdjust_2D = g_GameWork.config.optExtraBulletAdjust_2D % g_ExtraOptionsMenu_BulletMultMax;
                    break;
            }
        }

        // Leave menu.
        if ((g_Controller0->btnsClicked_10 & (g_GameWorkPtr->config.controllerConfig_0.cance |
                                            (ControllerFlag_L2 | ControllerFlag_R2 | ControllerFlag_L1 | ControllerFlag_R1))) &&
            g_GameWork.gameStateSteps[0] != OptionsMenuState_LeaveExtraOptions)
        {
            if (g_Controller0->btnsClicked_10 & g_GameWorkPtr->config.controllerConfig_0.cance)
            {
                Sd_PlaySfx(Sfx_Cancel, 0, 64);
            }
            else
            {
                Sd_PlaySfx(Sfx_Confirm, 0, 64);
            }

            ScreenFade_Start(true, false, false);
            g_GameWork.gameStateSteps[0] = OptionsMenuState_LeaveExtraOptions;
            g_SysWork.counters_1C[1]              = 0;
            g_GameWork.gameStateSteps[1] = 0;
            g_GameWork.gameStateSteps[2] = 0;
        }
*/
    }
}
