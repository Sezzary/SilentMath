#include "Framework.h"
#include "Psx.h"
#include "Game/Bodyprog/Events/Radio.h"

#include "Game/Bodyprog/Bodyprog.h"

#include "Game/Bodyprog/Events/GameSysStates.h"
#include "Game/Bodyprog/Sound/SoundSystem.h"

namespace Silent::Game
{
    void func_80037154(void) // 0x80037154
    {
        s32 i;

        for (i = 0; i < ARRAY_SIZE(g_RadioNpcInfos); i++)
        {
            g_RadioNpcInfos[i].closeNpcInfoIdx = NO_VALUE;
            g_RadioNpcInfos[i].idx             = NO_VALUE;
            g_RadioNpcInfos[i].field_3         = 0;
        }
    }

    void Game_RadioSoundStop(void) // 0x80037188
    {
        s32 i;

        for (i = 0; i < ARRAY_SIZE(g_RadioNpcInfos); i++)
        {
            g_RadioNpcInfos[i].prevIdx = NO_VALUE;
        }

        for (i = 0; i < ARRAY_SIZE(g_RadioNpcInfos); i++)
        {
            Sd_SfxStop(Sfx_RadioInterferenceLoop + i);
        }
    }
}
