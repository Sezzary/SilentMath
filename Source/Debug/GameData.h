#pragma once

namespace Silent::Debug
{
    /** These debug pointers get around the problem of PSX headers having dirty macros which are disruptive to the C++
     * standard library and other dependencies.
     */

    extern int* g_SysState;
    extern int* g_SysStateSteps;
    extern int* g_GameState;
    extern int* g_GameStateSteps;
}
