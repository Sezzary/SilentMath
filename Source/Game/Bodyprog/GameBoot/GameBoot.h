#pragma once

namespace Silent::Game
{
    void GameBoot_SavegameInitialize(s8 overlayId, s32 difficulty);

    /** @brief Initializes the player character model. */
    void GameBoot_PlayerInit(void);

    /** Loads a map file into `g_OvlDynamic`. */
    void GameBoot_MapLoad(s32 mapIdx);
}
