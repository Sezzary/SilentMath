#pragma once

#include <psyz.h>
#include <libetc.h>
#include <libgs.h>

namespace Silent::Game
{
    extern q19_12 g_PrevScreenFadeProgress;

    extern q19_12 g_ScreenFadeTimestep;

    // ====================
    // GLOBALS (BSS; Hack)
    // ====================
    // To match the order of the BSS segment, extern declarations
    // are required in a predetermined order.
    // This is done until a way to replicate `common`
    // segment behavior is found.

    extern s32 g_VBlanks;
    extern s32 g_UncappedVBlanks;
    //extern GsOT_TAG g_OtTags0[2][16];

    /** Main gameplay timer. Code may pause gameplay by setting this to 0.
     * Demos override this with the time from demo file to ensure demos are in sync.
     */
    extern q19_12 g_DeltaTime;

    //extern GsOT_TAG g_OtTags1[2][ORDERING_TABLE_SIZE];

    /** Gravity speed for this tick. */
    extern q19_12 g_GravitySpeed;

    /** Number of ticks since game start. */
    extern s32 g_TickCount;

    void Screen_RectInterlacedClear(s16 x, s16 y, s16 w, s16 h, u8 r, u8 g, u8 b);

    void Screen_Refresh(s32 screenWidth, bool isInterlaced);

    void Screen_Init(s32 screenWidth, bool isInterlaced);

    void Screen_XyPositionSet(s32 x, s32 y);

    void Screen_FadeDrawModeSet(DR_MODE* drMode);

    q19_12 Screen_FadeInProgressGet(void);

    void Screen_FadeUpdate(void);

    void Screen_CutsceneCameraStateUpdate(void);

    void Screen_VSyncCallback(void);
}
