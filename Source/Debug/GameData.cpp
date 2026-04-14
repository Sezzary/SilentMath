#include "Framework.h"
#include "Psx.h"
#include "Debug/GameData.h"

#include "Game/Bodyprog/Bodyprog.h"

using namespace Silent::Game;

namespace Silent::Debug
{
    int* g_SysState       = (int*)&g_SysWork.sysState;
    int* g_SysStateSteps  = (int*)&g_SysWork.sysStateSteps;
    int* g_SysCounters    = (int*)&g_SysWork.counters_1C;
    int* g_GameState      = (int*)&g_GameWork.gameState;
    int* g_GameStateSteps = (int*)&g_GameWork.gameStateSteps;
}
