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
        GameState_Boot_Update,
        NULL,//GameState_KonamiLogo_Update,
        NULL,//GameState_KcetLogo_Update,
        GameState_MovieIntroFadeIn_Update,
        GameState_AutoLoadSavegame_Update,
        GameState_MovieIntroAlternate_Update,
        GameState_MovieIntro_Update,
        GameState_MainMenu_Update,
        GameState_LoadSavegameScreen_Update,
        GameState_MovieOpening_Update,
        GameState_LoadScreen_Update,
        GameState_InGame_Update,
        GameState_MapEvent_Update,
        GameState_ExitMovie_Update,
        GameState_ItemScreens_Update,
        GameState_MapScreen_Update,
        GameState_LoadSavegameScreen_Update,
        GameState_DebugMoviePlayer_Update,
        GameState_Options_Update,
        NULL,//GameState_LoadStatusScreen_Update,
        GameState_LoadMapScreen_Update,
        GameState_Unk15_Update
    };

    void GameState_Boot_Update(void) // 0x80032D1C
    {
        // @todo There's so much stubbing. ;_;
    }
}
