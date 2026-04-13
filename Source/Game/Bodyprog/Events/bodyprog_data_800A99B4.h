#pragma once

namespace Silent::Game
{
    struct s_AreaLoadSfx;
    struct s_FsImageDesc;

    /** @brief SFX pair indices. Used for `SFX_PAIRS`. */
    enum e_SfxPairIdx
    {
        SfxPairIdx_0  = 0,
        SfxPairIdx_1  = 1,
        SfxPairIdx_2  = 2,
        SfxPairIdx_3  = 3,
        SfxPairIdx_4  = 4,
        SfxPairIdx_5  = 5,
        SfxPairIdx_6  = 6,
        SfxPairIdx_7  = 7,
        SfxPairIdx_8  = 8,
        SfxPairIdx_9  = 9,
        SfxPairIdx_10 = 10,
        SfxPairIdx_11 = 11,
        SfxPairIdx_12 = 12,
        SfxPairIdx_13 = 13,
        SfxPairIdx_14 = 14,
        SfxPairIdx_15 = 15,
        SfxPairIdx_16 = 16,
        SfxPairIdx_17 = 17,
        SfxPairIdx_18 = 18,
        SfxPairIdx_19 = 19,
        SfxPairIdx_20 = 20,
        SfxPairIdx_21 = 21,
        SfxPairIdx_22 = 22,
        SfxPairIdx_23 = 23,
        SfxPairIdx_24 = 24
    };

    extern const s_AreaLoadSfx SFX_PAIRS[25];

    extern s8 g_PaperMapFileIdxs[];

    extern s8 g_PaperMapMarkingFileIdxs[];

    extern char* D_800A99E4[8];

    extern s_FsImageDesc D_800A9A04;

    extern s32 D_800A9A0C; // Old IDB name `FS_AllFilesLoaded`, though FS code doesn't set it.

    extern s32 g_MapEventSysState; /** `e_InventoryItemId` */

    extern u32 g_MapEventParam;

    extern s32 g_MapEventLastUsedItem; /** `e_InventoryItemId` */

    /** Radio pitch state based on the distance from the player to an enemy. Range: `[0, 3]`. */
    extern s32 g_RadioPitchState;
}
