#include "Framework.h"
#include "Game/Bodyprog/Sys/GameMain.h"

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
        // @todo There's so much stubbing. ;_;
    }
}
