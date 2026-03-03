#pragma once

namespace Silent::Game
{
    extern q19_12 g_ScreenFadeTimestep;

    void Screen_FadeDrawModeSet(DR_MODE* drMode);

    q19_12 Screen_FadeInProgressGet(void);

    void Screen_FadeUpdate(void);
}
