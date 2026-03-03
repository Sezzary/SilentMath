#include "Framework.h"
#include "Psx.h"
#include "Game/Screens/Stream/Stream.h"

#include "Game/Bodyprog/Bodyprog.h"

#include "Application.h"
#include "Game/Bodyprog/Screen/ScreenDraw.h"
#include "Game/Bodyprog/Sys/FsScreens.h"
#include "Game/Bodyprog/Sys/Joy.h"
#include "Game/Bodyprog/Text/TextDraw.h"
#include "Game/Main/FileInfo.h"
#include "Input/Input.h"
//#include "bodyprog/libsd.h"

using namespace Silent::Input;

namespace Silent::Game
{
    void GameState_MovieIntroFadeIn_Update(void) // 0x801E2654
    {
        const auto& input = g_App.GetInput();

        switch (g_GameWork.gameStateStep_598[0])
        {
            case 0:
                //VSync(SyncMode_Wait8);
                ScreenFade_Start(true, true, false);
                GameFs_TitleGfxLoad();

                g_GameWork.gameStateStep_598[0]++;
                break;

            case 1:
                if (input.HasUserActionInput() || g_SysWork.counters_1C[0] > 300)
                {
                    ScreenFade_Start(false, false, false);
                    g_GameWork.gameStateStep_598[0] = 2;
                }
                break;

            case 2:
                if (ScreenFade_IsFinished())
                {
                    Fs_QueueWaitForEmpty();
                    Game_StateSetNext(GameState_MovieIntro);
                }
                break;
        }

        // "No memory card" image.
        //Screen_BackgroundImgDraw(D_800A900C);
    }

    void GameState_MovieIntro_Update(void) // 0x801E279C
    {
        s32 fileIdx = FILE_XA_C1_20670;

        if (g_GameWorkConst->config_0.optExtraOptionsEnabled_27 & (1 << 0))
        {
            fileIdx = FILE_XA_C2_20670;
        }

        open_main(fileIdx, 0);
        Game_StateSetNext(GameState_MainMenu);

        g_ScreenFadeTimestep = Q12(1.0f);
    }

    void GameState_MovieOpening_Update(void) // 0x801E2838
    {
        open_main(FILE_XA_M1_03500, 0);
        Game_StateSetNext(GameState_MainLoadScreen);
    }

    void GameState_ExitMovie_Update(void) // 0x801E28B0
    {
        Game_StateSetNext(GameState_InGame);
    }

    void GameState_DebugMoviePlayer_Update(void) // 0x801E2908
    {
        static s32 g_Debug_MoviePlayerIdx = 0; // 0x801E3F3C

        const auto& input = g_App.GetInput();

        if (input.GetAction(In::Cancel).IsClicked())
        {
            Game_StateSetNext(GameState_Unk16); // Changes to non-existent state 22 and crashes. Maybe removed debug menu.
        }

        if (input.GetAction(In::Left).IsPulsed(0.2f, 0.4f))
        {
            g_Debug_MoviePlayerIdx--;
        }

        if (input.GetAction(In::Right).IsPulsed(0.2f, 0.4f))
        {
            g_Debug_MoviePlayerIdx++;
        }

        if (input.GetAction(In::Enter).IsClicked())
        {
            open_main(FILE_XA_ZC_14392 - g_Debug_MoviePlayerIdx, 0);
        }
    }

    void GameState_MovieIntroAlternate_Update(void) // 0x801E2A24
    {
        open_main(FILE_XA_C1_20670, 2060); // Second param looks like file ID for `FILE_XA_M6_02112`, but is actually frame count?
        Game_StateSetNext(GameState_MainMenu);

        g_ScreenFadeTimestep = Q12(1.0f);
    }

    void open_main(s32 file_idx, s16 num_frames) // 0x801E2AA4
    {
        Fs_QueueWaitForEmpty();
        movie_main(NULL, num_frames, g_FileTable[file_idx].startSector_0_0);
    }

    void movie_main(char* file_name, s32 f_size, s32 sector) // 0x801E2B9C
    {
        // @todo Video streaming.
        /*do
        {
        }
        while (!input.GetAction(In::Enter).IsClicked() &&
            MainLoop_ShouldWarmReset() <= ResetType_None);*/
    }
}
