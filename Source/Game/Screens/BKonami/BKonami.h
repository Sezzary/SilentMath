#pragma once

namespace Silent::Game
{
    // Used by both `GameState_KcetLogo_MemCardCheck` and `GameState_KcetLogo_Update`.
    enum e_KcetLogoStateStep
    {
        KcetLogoStateStep_Init,
        KcetLogoStateStep_CheckMemCards,
        KcetLogoStateStep_NoMemCard,
        KcetLogoStateStep_NoMemCardFreeSpace,
        KcetLogoStateStep_NoSaveGame,
        KcetLogoStateStep_HasSavegame,
        KcetLogoStateStep_LogoDelay,
        KcetLogoStateStep_FinishAfterFade
    };

    enum e_KonamiLogoStateStep
    {
        KonamiLogoStateStep_Init,
        KonamiLogoStateStep_WaitForFade,
        KonamiLogoStateStep_LogoDelay,
        KonamiLogoStateStep_FinishAfterFade
    };

    /** Displays the Konami logo and starts loading base hero animations. */
    void GameState_KonamiLogo_Update(void);

    /** @brief Checks memory cards for free space & existing savegames.
     @return `e_KcetLogoStateStep`
    */
    s32 GameState_KcetLogo_MemCardCheck(void);

    /** Displays the KCET logo and starts loading various things. */
    void GameState_KcetLogo_Update(void);

    void BootScreen_KonamiScreenDraw(void);

    void BootScreen_KcetScreenDraw(void);
}
