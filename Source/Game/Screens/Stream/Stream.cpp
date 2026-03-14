#include "Framework.h"
#include "Psx.h"
#include "Game/Screens/Stream/Stream.h"

#include "Game/Bodyprog/Bodyprog.h"

#include "Application.h"
#include "Game/Bodyprog/Screen/ScreenData.h"
#include "Game/Bodyprog/Screen/ScreenDraw.h"
#include "Game/Bodyprog/Sys/FsScreens.h"
#include "Game/Bodyprog/Sys/Joy.h"
#include "Game/Bodyprog/Text/TextDraw.h"
#include "Game/Main/FileInfo.h"
#include "Input/Input.h"
#include "Renderer/Renderer.h"
#include "Utils/Video.h"
//#include "bodyprog/libsd.h"

using namespace Silent::Input;

namespace Silent::Game
{
    void GameState_MovieIntroFadeIn_Update() // 0x801E2654
    {
        const auto& input = g_App.GetInput();

        switch (g_GameWork.gameStateStep_598[0])
        {
            case 0:
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
    }

    void GameState_MovieIntro_Update() // 0x801E279C
    {
        const char* videoName = (g_GameWorkConst->config_0.optExtraOptionsEnabled_27 & (1 << 0)) ? "C1_20670.MPG" :
                                                                                                   "C2_20670.MPG";
        if (!movie_main(std::string(videoName), 0, 0))
        {
            Game_StateSetNext(GameState_MainMenu);
            g_ScreenFadeTimestep = Q12(1.0f);
        }
    }

    void GameState_MovieOpening_Update() // 0x801E2838
    {
        if (!movie_main("C1_20670.MPG", 0, 0))
        {
            Game_StateSetNext(GameState_MainLoadScreen);
        }
    }

    void GameState_ExitMovie_Update() // 0x801E28B0
    {
        Game_StateSetNext(GameState_InGame);
    }

    void GameState_DebugMoviePlayer_Update() // 0x801E2908
    {
        // @stub
    }

    void GameState_MovieIntroAlternate_Update() // 0x801E2A24
    {
        if (!movie_main("C1_20670.MPG", 0, 0))
        {
            Game_StateSetNext(GameState_MainMenu);
            g_ScreenFadeTimestep = Q12(1.0f);
        }
    }

    void open_main(s32 file_idx, s16 num_frames) // 0x801E2AA4
    {
        //Fs_QueueWaitForEmpty();
        //movie_main(nullptr, num_frames, g_FileTable[file_idx].startSector_0_0);
    }

    bool movie_main(const std::string& file_name, s32 f_size, s32 sector) // 0x801E2B9C
    {
        const auto& input    = g_App.GetInput();
        auto&       renderer = g_App.GetRenderer();
        auto&       video    = g_App.GetVideo();

        // Start playing new video.
        if (!video.IsLoaded() || file_name != video.GetName())
        {
            video.Play(file_name);
        }
        // Update active video playback.
        else
        {
            if (!video.IsPlaying() || input.GetAction(In::Enter).IsClicked())
            {
                video.Stop();
                return false;
            }
            else
            {
                video.Update(1.0f / 30.0f);//Q12_TO_FLT(g_DeltaTime));
            }
        }

        // Submit fullscreen video sprite.
        float aspect = 4.0f / 3.0f;//video.GetAspectRatio(); // @todo PL_MPEG returns wrong info.
        auto  scale  = Vector2(std::max(aspect, 1.0f), std::min(aspect, 1.0f));
        auto  sprite = Sprite2d::CreateSprite2d(video.GetName(), Vector2::Zero, Vector2::One,
                                                SCREEN_SPACE_RES / 2.0f, DEG_TO_RAD(0.0f), scale, Color::White,
                                                100, AlignMode::Center, ScaleMode::ShortEdge, BlendMode::Opaque);
        renderer.SubmitSprite2d(sprite);
        return true;
    }
}
