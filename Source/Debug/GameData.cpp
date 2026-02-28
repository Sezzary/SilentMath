#include "Framework.h"
#include "Psx.h"
#include "Debug/GameData.h"

#include "Game/Bodyprog/Bodyprog.h"

using namespace Silent::Game;

namespace Silent::Debug
{
    int* g_SysState       = (int*)&g_SysWork.sysState_8;
    int* g_SysStateSteps  = &g_SysWork.sysStateStep_C[0];
    int* g_GameState      = (int*)&g_GameWork.gameState_594;
    int* g_GameStateSteps = &g_GameWork.gameStateStep_598[0];
}
