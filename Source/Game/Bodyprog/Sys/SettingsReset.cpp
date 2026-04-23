#include "Framework.h"
#include "Game/Bodyprog/Bodyprog.h"
#include "Game/Bodyprog/Sys/SettingsReset.h"

#include "Game/Bodyprog/Screen/ScreenDraw.h"
#include "Game/Bodyprog/Sound/SoundSystem.h"

namespace Silent::Game
{
    void Settings_ScreenAndVolUpdate() // 0x800333CC
    {
        Screen_XyPositionSet(g_GameWork.config.optScreenPosX_1C, g_GameWork.config.optScreenPosY_1D);
        SD_Call((g_GameWork.config.optSoundType_1E != 0) ? AudioMode_Mono : AudioMode_Stereo);
        //Sd_SetVolume(OPT_SOUND_VOLUME_MAX, g_GameWork.config.optVolumeBgm_1F, g_GameWork.config.optVolumeSe_20);
    }

    void Settings_RestoreDefaults() // 0x8003342C
    {
        g_GameWork.config.optExtraWeaponCtrl_23 = 1;
        g_GameWork.config.optBrightness_22      = 3;

        Settings_RestoreControlDefaults(0);

        g_GameWork.config.optVibrationEnabled_21 = OPT_VIBRATION_ENABLED;
        g_GameWork.config.optVolumeBgm_1F        = OPT_SOUND_VOLUME_MAX;
        g_GameWork.config.optVolumeSe_20         = OPT_SOUND_VOLUME_MAX;

        Settings_ScreenAndVolUpdate();

        g_GameWork.config.optExtraBloodColor_24 = 0;
    }

    void Settings_RestoreControlDefaults(s32 configIdx) // 0x80033480
    {
        u32  i;
        u16* ptr;

        static const s_ControllerConfig DEFAULT_CONTROLLER_CONFIGS[3] =
        {
            {
                .enter      = ControllerFlag_Start | ControllerFlag_Cross,
                .cance     = ControllerFlag_Triangle | ControllerFlag_Circle | ControllerFlag_Square,
                .skip       = ControllerFlag_Start,
                .action     = ControllerFlag_Cross,
                .aim        = ControllerFlag_R2,
                .light      = ControllerFlag_Circle,
                .run        = ControllerFlag_Square,
                .view       = ControllerFlag_L2,
                .stepLeft  = ControllerFlag_L1,
                .stepRight = ControllerFlag_R1,
                .pause     = ControllerFlag_Start,
                .item      = ControllerFlag_Select,
                .map       = ControllerFlag_Triangle,
                .option    = ControllerFlag_None
            },
            {
                .enter      = ControllerFlag_Start | ControllerFlag_Cross,
                .cance     = ControllerFlag_Triangle | ControllerFlag_Circle | ControllerFlag_Square,
                .skip       = ControllerFlag_Start,
                .action     = ControllerFlag_Cross,
                .aim        = ControllerFlag_R1,
                .light      = ControllerFlag_Circle,
                .run        = ControllerFlag_Square,
                .view       = ControllerFlag_L1,
                .stepLeft  = ControllerFlag_L2,
                .stepRight = ControllerFlag_R2,
                .pause     = ControllerFlag_Start,
                .item      = ControllerFlag_Select,
                .map       = ControllerFlag_Triangle,
                .option    = ControllerFlag_None
            },
            {
                .enter      = ControllerFlag_Start | ControllerFlag_Cross,
                .cance     = ControllerFlag_Triangle | ControllerFlag_Circle | ControllerFlag_Square,
                .skip       = ControllerFlag_Start,
                .action     = ControllerFlag_Cross,
                .aim        = ControllerFlag_R2,
                .light      = ControllerFlag_Circle,
                .run        = ControllerFlag_Square,
                .view       = ControllerFlag_L2,
                .stepLeft  = ControllerFlag_L1,
                .stepRight = ControllerFlag_R1,
                .pause     = ControllerFlag_Start,
                .item      = ControllerFlag_Triangle,
                .map       = ControllerFlag_Select,
                .option    = ControllerFlag_None
            }
        };

        for (i = 0, ptr = (u16*)&g_GameWorkPtr->config.controllerConfig_0; i < INPUT_ACTION_COUNT; i++, ptr++)
        {
            *ptr = (&DEFAULT_CONTROLLER_CONFIGS[configIdx].enter)[i];
        }
    }
}
