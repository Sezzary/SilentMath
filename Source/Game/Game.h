#pragma once

#include "Psx.h"

#include "Game/Maps/Characters/Harry.h"

namespace Silent::Game
{
    struct s_AnmHeader;
    struct s_Model;

    constexpr int    TICKS_PER_SECOND = 60;                                        /** Game has a variable timestep with 60 ticks max. */
    constexpr q19_12 TIMESTEP_30_FPS  = Q12(1.0f / (float)(TICKS_PER_SECOND / 2)); /** Timestep at 30 FPS. */
    constexpr q19_12 TIMESTEP_60_FPS  = Q12(1.0f / (float)(TICKS_PER_SECOND));     /** Timestep at 60 FPS. */

    constexpr int SCREEN_WIDTH                   = 320;
    constexpr int SCREEN_HEIGHT                  = 240;
    constexpr int FRAMEBUFFER_HEIGHT_PROGRESSIVE = 224;
    constexpr int FRAMEBUFFER_HEIGHT_INTERLACED  = FRAMEBUFFER_HEIGHT_PROGRESSIVE * 2;
    constexpr int ORDERING_TABLE_SIZE            = 2048;

    constexpr int NPC_COUNT_MAX             = 6;
    constexpr int NPC_BONE_COUNT_MAX        = 10 * NPC_COUNT_MAX;
    constexpr int CHARA_GROUP_COUNT         = 4; /** While up to 6 NPCs and a player can exist in the game world, only 4 different character types (including the player) can be loaded at a time. */
    constexpr int INVENTORY_ITEM_COUNT_MAX  = 40;
    constexpr int INVENTORY_ITEM_GROUP_SIZE = 32; /** Number of `e_InventoryItemId`s per `e_InventoryItemGroup`. */
    constexpr int INPUT_ACTION_COUNT        = 14;
    constexpr int CONTROLLER_COUNT_MAX      = 2;

    constexpr int DEFAULT_PICKUP_ITEM_COUNT      = 1;
    constexpr int HANDGUN_AMMO_PICKUP_ITEM_COUNT = 15;
    constexpr int SHOTGUN_AMMO_PICKUP_ITEM_COUNT = 6;
    constexpr int RIFLE_AMMO_PICKUP_ITEM_COUNT   = 6;

    constexpr int  DEFAULT_MAP_MESSAGE_LENGTH     = 99;
    constexpr int  MAP_MESSAGE_DISPLAY_ALL_LENGTH = 400;  /** Long string length is used to display a whole message instantly without a rollout. */
    constexpr char GLYPH_TABLE_ASCII_OFFSET       = '\''; /** Subtracted from ASCII bytes to get index to some string-related table. */

    constexpr q19_12 CHUNK_CELL_SIZE = Q12(40.0f);

    constexpr int BGM_LAYER_COUNT = 9;

    /** @brief Converts a floating-point X screen position in percent to a fixed-point X screen coodinate. */
    #define SCREEN_POSITION_X(percent) \
        (s32)(SCREEN_WIDTH * ((percent) / 100.0f))

    /** @brief Converts a floating-point Y screen position in percent to a fixed-point Y screen coodinate. */
    #define SCREEN_POSITION_Y(percent) \
        (s32)(SCREEN_HEIGHT * ((percent) / 100.0f))

    /** @brief Checks if a specified map has been collected. */
    #define HAS_MAP(mapIdx) \
        ((((u32*)&g_SavegamePtr->hasMapsFlags)[(mapIdx) / 32] >> ((mapIdx) % 32)) & (1 << 0))

    // TODO: Name might be wrong, but these have something to do with held item meshes.
    // First index is the mesh variant, second is the container of meshes (not bone index in skeleton)?
    // Data addresses are hardcoded.
    /** @brief Packs a model bone containing a mesh variant index and ??? in a single value.
     *
     * @param variantIdx Mesh variant index.
     * @param idx1 ???
     * @return Packed model bone containing a mesh variant index and ???
     */
    #define MODEL_BONE(variantIdx, idx1) \
        (s16)((variantIdx) | ((idx1) << 4))

    /** @brief Retrieves the bone mesh variant index from a packed model bone.

    *
    * @param modelBone Packed model bone containing  a mesh variant index and ???
    * @return Bone mesh variant index.
    */
    #define MODEL_BONE_MESH_VARIANT_IDX_GET(modelBone) \
        ((modelBone) & 0xF)


    /** @brief Retrieves ???
     *
     * @param modelBone Packed model bone containing a mesh variant index and ???
     * @return Unknown second index.
     */
    #define MODEL_BONE_IDX_1_GET(modelBone) \
        ((modelBone) & 0xF0)

    /** @brief Packs a weapon attack containing a weapon ID and attack input type.
     *
     * @param weaponId Weapon ID.
     * @param attackInputType Attack input type.
     * @return Packed weapon attack containing a weapon ID and attack input type.
     */
    #define WEAPON_ATTACK(weaponId, attackInputType) \
        ((weaponId) + ((attackInputType) * 10))

    /** @brief Retrieves the weapon ID from a packed weapon attack.
     *
     * @param weaponAttack Packed weapon attack containing a weapon ID and attack input type.
     * @return Weapon ID.
     */
    #define WEAPON_ATTACK_ID_GET(weaponAttack) \
        ((weaponAttack) % 10)

    /** @brief Packs an animation status containing an animation index and active flag.
     * `isActive` is `false` in the blend phase and `true` in the playback phase.
     *
     * @param animIdx Animation index.
     * @param isActive Active status. (`bool`).
     * @return Packed animation status containing the animation index and active flag.
     */
    #define ANIM_STATUS(animIdx, isActive) \
        (((animIdx) << 1) | ((isActive) ? (1 << 0) : 0))

    /** @brief Retrieves the animation index from a packed animation status.
     *
     * @param animStatus Packed animation status containing an animation index and active flag.
     * @return Anim index.
     */
    #define ANIM_STATUS_IDX_GET(animStatus) \
        ((animStatus) >> 1)

    /** @brief Checks if an animation is active.
     *
     * @param animStatus Packed animation status containing an animation index and active flag.
     * @return `true` if active, `false` otherwise.
     */
    #define ANIM_STATUS_IS_ACTIVE(animStatus) \
        ((animStatus) & (1 << 0))

    /** @brief Checks if an animation time is within the keyframe index range `[low, high]`.
     *
     * @param animTime Animation time to check (Q*.12).
     * @param low Low keyframe index.
     * @param high High keyframe index.
     * @return `true` if the animation time is within the keyframe index range, `false` otherwise.
     */
    #define ANIM_TIME_RANGE_CHECK(animTime, low, high) \
        (FP_FROM(animTime, Q12_SHIFT) >= (low) && FP_FROM(animTime, Q12_SHIFT) <= (high))

    /** @brief Gets the relative keyframe index of an animation time with a base offset.
     *
     * @param animTime Animation time (Q*.12).
     * @param baseOffset Base keyframe index offset.
     * @return Relative keyframe index.
     */
    #define ANIM_TIME_REL_KEYFRAME_IDX_GET(animTime, baseOffset) \
        (FP_FROM(animTime, Q12_SHIFT) - (baseOffset))

    /** @brief Creates a bitmask with a contiguous range of bits set.
     * For use with `s_PlayerExtra::disabledAnimBones`.
     *
     * Generates an `unsigned int` mask with all bits in the range `[fromInclusive, toInclusive]` set.
     *
     * For example:
     * - `BITMASK_RANGE(0, 2)` -> 0b000...0111 (decimal 7).
     * - `BITMASK_RANGE(4, 11)` -> 0b000...111111110000 (decimal 4080).
     *
     * @param fromInclusive Index of the lowest bit to set (0 = least significant bit).
     * @param toInclusive Index of the highest bit to set.
     * @return Bitmask with the specified range of bits set to 1 (`unsigned int`).
     */
    #define BITMASK_RANGE(fromInclusive, toInclusive) \
        (((~0u << (fromInclusive)) & ~(~0u << ((toInclusive) + 1))))

    /** @brief Packs a screen fade status containing a fade state and white flag.
     * See `g_Screen_FadeStatus` for bit layout.
     *
     * @param state Screen fade state.
     * @param isWhite White status (`bool`).
     * @return Packed screen fade status containing a fade state and white flag.
     */
    #define SCREEN_FADE_STATUS(state, isWhite) \
        ((state) | ((isWhite) ? (1 << 3) : 0))

    /** @brief Checks if the screen fade isn't in progress (finished step) by masking away the color bit.
     *
     * @return `true` if finished, `false` if still in progress.
     */
    #define ScreenFade_IsFinished() \
        ((g_Screen_FadeStatus & 0x7) == ScreenFadeState_FadeOutComplete)

    /** @brief Checks if the screen fade isn't in progress (idle step) without masking away the color bit.
     *
     * @return `true` if idle, `false` otherwise.
     */
    #define ScreenFade_IsNone() \
        (g_Screen_FadeStatus == ScreenFadeState_None)

    /** @brief Checks if the screen fade is white.
     * See `g_Screen_FadeStatus` for bit layout.
     *
     * @param fadeStatus Packed screen fade status containing a fade state and white flag.
     * @return `true` if white, `false` if black.
     */
    #define IS_SCREEN_FADE_WHITE(fadeStatus) \
        ((fadeStatus) & (1 << 3))

    /** @brief Starts a screen fade in/out.
     *
     * @param reset `true` to reset fade progress to 0, `false` to keep it.
     *              Speculation: Skipping the screen fade progress step is a mistake. It still works because once a fade
     *              is finished, the progress variable will be reset to 0 anyway.
     * @param fadeIn `true` for fade in, `false` for fade out.
     * @param isWhite `true` for white fade, `false` for black fade.
     */
    #define ScreenFade_Start(reset, fadeIn, isWhite)                                                                \
        g_Screen_FadeStatus = (((((reset) == true) ? ScreenFadeState_FadeOutStart : ScreenFadeState_FadeOutSteps) + \
                                (((fadeIn) == true) ? 4 : 0)) |                                                     \
                               (((isWhite) == true) ? (1 << 3) : 0))

    /** @brief Resets the screen fade. */
    #define ScreenFade_Reset() \
        g_Screen_FadeStatus = ScreenFadeState_Reset

    /** @brief Resets the custom screen fade timestep back to zero, disregarding the color bit. */
    #define ScreenFade_ResetTimestep() \
        g_Screen_FadeStatus = ScreenFadeState_ResetTimestep

    #define MAP_CHUNK_CHECK_VARIABLE_DECL() \
        s32 __chunkIdx

    /** @hack This macro requires a variable `s32 __chunkIdx` to be declared before using it.
     * The macro `MAP_CHUNK_CHECK_VARIABLE_DECL` declares that variable and must be called before this macro.
     * The first argument is `vx` or `vz`, which is used as the component name in `VECTOR3`.
     * @bug Some maps appear to have a bug where the negative position check will never be true because they check
     * if the chunk index will be a positive number. Seems like they forgot to use `ABS`?
     */
    #define PLAYER_IN_MAP_CHUNK(comp, x0, x1, x2, x3)                                               \
        (__chunkIdx = g_SysWork.playerWork.player.position.comp / Q12(40.0f),                       \
        ((g_SysWork.playerWork.player.position.comp >  Q12(0.0f) && (__chunkIdx + (x0)) == (x1)) || \
        (g_SysWork.playerWork.player.position.comp  <= Q12(0.0f) && (__chunkIdx + (x2)) == (x3))))

    #define PLAYER_NOT_IN_MAP_CHUNK(comp, x0, x1, x2, x3)                                           \
        (__chunkIdx = g_SysWork.playerWork.player.position.comp / Q12(40.0f),                       \
        ((g_SysWork.playerWork.player.position.comp >  Q12(0.0f) && (__chunkIdx + (x0)) != (x1)) || \
        (g_SysWork.playerWork.player.position.comp  <= Q12(0.0f) && (__chunkIdx + (x2)) != (x3))))

    #define MAP_CHUNK_CHECK_VARIABLE_DECL_2() \
        s32 __chunkIdx2

    #define PLAYER_IN_MAP_CHUNK_2(comp, x0, x1, x2, x3)                                             \
        (__chunkIdx2 = g_SysWork.playerWork.player.position.comp / Q12(40.0f),                      \
        ((g_SysWork.playerWork.player.position.comp >  Q12(0.0f) && (__chunkIdx2 + (x0)) < (x1)) || \
        (g_SysWork.playerWork.player.position.comp  <= Q12(0.0f) && (__chunkIdx2 + (x2)) < (x3))))

    #define PLAYER_NEAR_POS(comp, base, tol)                                                                                                             \
        (((g_SysWork.playerWork.player.position.comp - Q12(base)) >= Q12(0.0f)) ? ((g_SysWork.playerWork.player.position.comp - Q12(base)) < Q12(tol)) : \
                                                                                  ((Q12(base) - g_SysWork.playerWork.player.position.comp) < Q12(tol)))

    #define MIN_OFFSET(x, neg, pos) \
        ((((x) + (-neg)) <= ((x) + (pos))) ? ((x) - (neg)) : ((x) + (pos)))

    #define MAX_OFFSET(x, neg, pos) \
        ((((x) - (neg)) > ((x) + (pos))) ? ((x) - (neg)) : ((x) + (pos)))

    /** @brief Packs XZ cell coordinates in a single value.
     *
     * @param x X cell coordinate.
     * @param z Z cell coordinate.
     * @return Packed XZ cell coordinates.
     */
    #define CELL_XZ(x, z) \
        ((x) + ((z) << 8))

    #define HAS_FLAG(ptr, idx) \
        ((((u32*)ptr)[(idx) >> 5] >> ((idx) & 0x1F)) & (1 << 0))

    #define SET_FLAG(ptr, idx) \
        ((((u32*)ptr)[(idx) >> 5] |= (1 << 0) << ((idx) & 0x1F)))

    #define CLEAR_FLAG(ptr, idx) \
        ((((u32*)ptr)[(idx) >> 5] &= ~((1 << 0) << ((idx) & 0x1F))))

    /** @brief Gets the `e_InventoryItemGroup` for an `e_InventoryItemId`.
     * Divides the item ID by 32 (`INVENTORY_ITEM_GROUP_SIZE`), using a bitwise shift to match.
     *
     * @param itemId Item ID to process.
     * @return Inventory item group.
     */
    #define INVENTORY_ITEM_GROUP(itemId) \
        ((itemId) >> 5)

    /** @brief Gets the index of an `e_InventoryItemId` inside the group it belongs to.
     * Modulos the item ID by 32 (`INVENTORY_ITEM_GROUP_SIZE`), using AND to match.
     * E.g. `itemId` 65 would be index 1, group 2.
     *
     * @param itemId Item ID to process.
     * @return Inventory item group.
     */
    #define INVENTORY_ITEM_GROUP_ID(itemId) \
        ((itemId) & 0x1F)

    /** @brief Gets the `e_InventoryItemId` of the ammo for a given weapon item.
     *
     * @param itemId Weapon item ID to process.
     * @return Inventory item group.
     */
    #define INVENTORY_WEAPON_AMMO_ID(weaponId) \
        ((weaponId) + INVENTORY_ITEM_GROUP_SIZE)

    /** @brief Gets the `e_InventoryItemId` of the weapon for a given ammo item.
     *
     * @param ammoId Ammo ID to process.
     * @return Inventory item group.
     */
    #define INVENTORY_AMMO_WEAPON_ID(ammoId) \
        ((ammoId) - INVENTORY_ITEM_GROUP_SIZE)

    /** @brief Character group flags. Used by `s_SysWork::charaGroupFlags`. */
    typedef enum _CharaGroupFlags
    {
        CharaGroupFlag_None = 0,
        CharaGroupFlag_0    = 1 << 0,
        CharaGroupFlag_1    = 1 << 1
    } e_CharaGroupFlags;

    /** @brief Character collision states. */
    typedef enum _CharaCollisionState
    {
        CharaCollisionState_Ignore = 0,
        CharaCollisionState_Player = 1,
        CharaCollisionState_2      = 2,
        CharaCollisionState_Npc    = 3,
        CharaCollisionState_4      = 4,
        CharaCollisionState_5      = 5
    } e_CharaCollisionState;

    /** @brief Sync modes used by `DrawSync` and `VSync`. */
    enum e_SyncMode
    {
        SyncMode_Count     = -1,
        SyncMode_Wait      = 0,
        SyncMode_Immediate = 1,
        SyncMode_Wait2     = 2,
        SyncMode_Wait3     = 3,
        SyncMode_Wait8     = 8
    };

    enum e_DmsIntervalState
    {
        DmsIntervalState_Interpolating = 0,
        DmsIntervalState_SingleFrame   = 1,
        DmsIntervalState_Ending        = 2
    };

    /** @brief Screen fade states used by `g_Screen_FadeStatus`. The flow is not linear. */
    enum e_ScreenFadeState
    {
        ScreenFadeState_Reset           = 0,
        ScreenFadeState_None            = 1,
        ScreenFadeState_FadeOutStart    = 2,
        ScreenFadeState_FadeOutSteps    = 3,
        ScreenFadeState_ResetTimestep   = 4,
        ScreenFadeState_FadeOutComplete = 5,
        ScreenFadeState_FadeInStart     = 6,
        ScreenFadeState_FadeInSteps     = 7
    };

    /** @brief Map message indices.
     *
     * @note Each map has its own messages, with the first 15 hardcoded to be the same.
     */
    enum e_MapMsgIdx
    {
        MapMsgIdx_Yes               = 0,
        MapMsgIdx_No                = 1,
        MapMsgIdx_SaveGame          = 2, // "Someday, someone may experience..."
        MapMsgIdx_NoMap             = 3,
        MapMsgIdx_TooDarkForMap     = 4,
        MapMsgIdx_FirstAidSelect    = 5,
        MapMsgIdx_HealthDrinkSelect = 6,
        MapMsgIdx_AmpouleSelect     = 7,
        MapMsgIdx_HandgunAmmoSelect = 8,
        MapMsgIdx_RifleAmmoSelect   = 9,
        MapMsgIdx_ShotgunAmmoSelect = 10,
        MapMsgIdx_DoorJammed        = 11,
        MapMsgIdx_DoorLocked        = 12,
        MapMsgIdx_DoorUnlocked      = 13,
        MapMsgIdx_NowMaking         = 14 // @unused?
    };

    enum e_MapMsgCode
    {
        MapMsgCode_None        = 0,
        MapMsgCode_Select2     = 2,
        MapMsgCode_Select3     = 3,
        MapMsgCode_Select4     = 4,
        MapMsgCode_DisplayAll  = 20,
        MapMsgCode_SetByT      = 88,
        MapMsgCode_AlignCenter = 99
    };

    /** @brief Map message states.
     *
     * Return states used by `MapMsg_Draw`.
     */
    enum e_MapMsgState
    {
        MapMsgState_Finish       = NO_VALUE, /** Initial setup complete, cutscene timer complete, or input to continue from user received. */
        MapMsgState_Idle         = 0,        /** Continue displaying message. */
        MapMsgState_SelectEntry0 = 1,        /** First entry selected in selection dialog. */
        MapMsgState_SelectEntry1 = 2,        /** Second entry selected in selection dialog. */
        MapMsgState_SelectEntry2 = 3         /** Third entry selected in selection dialog. */
    };

    enum e_MapMsgAudioLoadBlock
    {
        MapMsgAudioLoadBlock_None = 0, // TODO: Some code checks only for bit 0, so it should mean something.
        MapMsgAudioLoadBlock_Unk1 = 1,
        MapMsgAudioLoadBlock_J2   = 3  // `J2` map messages set this, causing voice audio to not load.
                                    // `J2` cutscenes use single audio file for all lines (e.g. video tape cutscene).
    };

    /** @brief Playable map indices. Used for binary overlays. */
    enum e_MapIdx
    {
        MapIdx_MAP0_S00 = 0,
        MapIdx_MAP0_S01 = 1,
        MapIdx_MAP0_S02 = 2,
        MapIdx_MAP1_S00 = 3,
        MapIdx_MAP1_S01 = 4,
        MapIdx_MAP1_S02 = 5,
        MapIdx_MAP1_S03 = 6,
        MapIdx_MAP1_S04 = 7,
        MapIdx_MAP1_S05 = 8,
        MapIdx_MAP1_S06 = 9,
        MapIdx_MAP2_S00 = 10,
        MapIdx_MAP2_S01 = 11,
        MapIdx_MAP2_S02 = 12,
        MapIdx_MAP2_S03 = 13,
        MapIdx_MAP2_S04 = 14,
        MapIdx_MAP3_S00 = 15,
        MapIdx_MAP3_S01 = 16,
        MapIdx_MAP3_S02 = 17,
        MapIdx_MAP3_S03 = 18,
        MapIdx_MAP3_S04 = 19,
        MapIdx_MAP3_S05 = 20,
        MapIdx_MAP3_S06 = 21,
        MapIdx_MAP4_S00 = 22,
        MapIdx_MAP4_S01 = 23,
        MapIdx_MAP4_S02 = 24,
        MapIdx_MAP4_S03 = 25,
        MapIdx_MAP4_S04 = 26,
        MapIdx_MAP4_S05 = 27,
        MapIdx_MAP4_S06 = 28,
        MapIdx_MAP5_S00 = 29,
        MapIdx_MAP5_S01 = 30,
        MapIdx_MAP5_S02 = 31,
        MapIdx_MAP5_S03 = 32,
        MapIdx_MAP6_S00 = 33,
        MapIdx_MAP6_S01 = 34,
        MapIdx_MAP6_S02 = 35,
        MapIdx_MAP6_S03 = 36,
        MapIdx_MAP6_S04 = 37,
        MapIdx_MAP6_S05 = 38,
        MapIdx_MAP7_S00 = 39,
        MapIdx_MAP7_S01 = 40,
        MapIdx_MAP7_S02 = 41,
        MapIdx_MAP7_S03 = 42,
        MapIdx_MAPT_S00 = 43, // } @unused Empty test maps. Only some code references remain and `HB_MTS00.ANM`/`HB_MTX00.ANM` anim files.
        MapIdx_MAPX_S00 = 44  // }
    };

    /** @brief Paper map indices. Used for the navigation map screen. */
    enum e_PaperMapIdx
    {
        PaperMapIdx_OtherPlaces    = 0,
        PaperMapIdx_OldTown        = 1,
        PaperMapIdx_FogCentralTown = 2,
        PaperMapIdx_AltCentralTown = 3,
        PaperMapIdx_ResortTown     = 4,
        PaperMapIdx_FogSchoolBF    = 5,
        PaperMapIdx_FogSchool1F    = 6,
        PaperMapIdx_FogSchool2F    = 7,
        PaperMapIdx_FogSchoolRF    = 8,
        PaperMapIdx_AltSchoolBF    = 9,
        PaperMapIdx_AltSchool1F    = 10,
        PaperMapIdx_AltSchool2F    = 11,
        PaperMapIdx_AltSchoolRF    = 12,
        PaperMapIdx_FogSewer1F     = 13,
        PaperMapIdx_FogSewer2F     = 14,
        PaperMapIdx_AltSewer       = 15,
        PaperMapIdx_FogHospitalBF  = 16,
        PaperMapIdx_FogHospital1F  = 17,
        PaperMapIdx_FogHospital2F  = 18,
        PaperMapIdx_FogHospital3F  = 19,
        PaperMapIdx_AltHospitalBF  = 20,
        PaperMapIdx_AltHospital1F  = 21,
        PaperMapIdx_AltHospital2F  = 22,
        PaperMapIdx_AltHospital3F  = 23
    };

    // Temp name. Related to music.
    enum _BgmStatusFlags
    {
        BgmStatusFlag_None        = 0,
        BgmStatusFlag_Pause       = 1 << 0,
        BgmStatusFlag_ApplyMute   = 1 << 1,
        BgmStatusFlag_RadioActive = 1 << 2,
        BgmStatusFlag_Duck        = 1 << 3,
        BgmStatusFlag_4           = 1 << 4,
        BgmStatusFlag_VoiceDialog = 1 << 5,
        BgmStatusFlag_6           = 1 << 6, // Something to do with the radio?
        BgmStatusFlag_RequestMute = 1 << 7
    };

    // Temp name.
    enum e_UnkSysFlags
    {
        UnkSysFlag_None = 0,
        UnkSysFlag_0    = 1 << 0,
        UnkSysFlag_1    = 1 << 1, // Demo in progress?

        UnkSysFlag_3    = 1 << 3,
        UnkSysFlag_4    = 1 << 4,
        UnkSysFlag_5    = 1 << 5, /** Related to camera. */
        UnkSysFlag_6    = 1 << 6,
        UnkSysFlag_7    = 1 << 7, /** Set by `SysState_Gameplay_Update` when opening the menu and cleared when returning to `SysState_Gameplay` once screen fade has completed. */
        UnkSysFlag_8    = 1 << 8,
        UnkSysFlag_9    = 1 << 9,
        UnkSysFlag_10   = 1 << 10
    };

    /** @brief System process flags. */
    enum e_ProcessFlags
    {
        ProcessFlag_None              = 0,
        ProcessFlag_RoomTransition    = 1 << 0,
        ProcessFlag_OverlayTransition = 1 << 1,
        ProcessFlag_NewGame           = 1 << 2,
        ProcessFlag_LoadSave          = 1 << 3,
        ProcessFlag_Continue          = 1 << 4,
        ProcessFlag_BootDemo          = 1 << 5
    };

    enum e_ControllerFlags
    {
        ControllerFlag_None         = 0,
        ControllerFlag_Select       = 1 << 0,
        ControllerFlag_L3           = 1 << 1,
        ControllerFlag_R3           = 1 << 2,
        ControllerFlag_Start        = 1 << 3,
        ControllerFlag_DpadUp       = 1 << 4,
        ControllerFlag_DpadRight    = 1 << 5,
        ControllerFlag_DpadDown     = 1 << 6,
        ControllerFlag_DpadLeft     = 1 << 7,
        ControllerFlag_L2           = 1 << 8,
        ControllerFlag_R2           = 1 << 9,
        ControllerFlag_L1           = 1 << 10,
        ControllerFlag_R1           = 1 << 11,
        ControllerFlag_Triangle     = 1 << 12,
        ControllerFlag_Circle       = 1 << 13,
        ControllerFlag_Cross        = 1 << 14,
        ControllerFlag_Square       = 1 << 15,
        ControllerFlag_LStickUp2    = 1 << 16,
        ControllerFlag_LStickRight2 = 1 << 17,
        ControllerFlag_LStickDown2  = 1 << 18,
        ControllerFlag_LStickLeft2  = 1 << 19,
        ControllerFlag_RStickUp     = 1 << 20,
        ControllerFlag_RStickRight  = 1 << 21,
        ControllerFlag_RStickDown   = 1 << 22,
        ControllerFlag_RStickLeft   = 1 << 23,
        ControllerFlag_LStickUp     = 1 << 24,
        ControllerFlag_LStickRight  = 1 << 25,
        ControllerFlag_LStickDown   = 1 << 26,
        ControllerFlag_LStickLeft   = 1 << 27
    };

    /** @brief Character flags. Used by `s_SubCharacter::flags`. */
    enum e_CharaFlags
    {
        CharaFlag_None          = 0,
        CharaFlag_PadlockBroken = 1 << 0, /** Only used by Padlock character. */
        CharaFlag_1             = 1 << 1, // Related to being damaged. Maybe to recoil or notify of danger?
        CharaFlag_Hit           = 1 << 2, // Associated with recoil-causing hit?
        CharaFlag_3             = 1 << 3,
        CharaFlag_4             = 1 << 4, // Camera-related. Only used by Stalker? Maybe only for alley scenario?
        CharaFlag_Damaged       = 1 << 5,
        CharaFlag_Dead          = 1 << 6, // Unsure.
        CharaFlag_7             = 1 << 7,
        CharaFlag_8             = 1 << 8  // Only set for bosses and NPCs in special scenarios.
    };

    /** @brief Character animation flags. */
    enum e_AnimFlags
    {
        AnimFlag_None     = 0,
        AnimFlag_Unlocked = 1 << 0,
        AnimFlag_Visible  = 1 << 1
    };

    /** @brief State IDs used by the main game loop. The values are used as indices into the `g_GameStateUpdateFuncs` function array. */
    enum e_GameState
    {
        GameState_Init                = 0,
        GameState_KonamiLogo          = 1,
        GameState_KcetLogo            = 2,
        GameState_MovieIntroFadeIn    = 3,
        GameState_AutoLoadSavegame    = 4,
        GameState_MovieIntroAlternate = 5,
        GameState_MovieIntro          = 6,
        GameState_MainMenu            = 7,
        GameState_LoadSavegameScreen  = 8,
        GameState_MovieOpening        = 9,
        GameState_MainLoadScreen      = 10,
        GameState_InGame              = 11,
        GameState_MapEvent            = 12,
        GameState_ExitMovie           = 13,
        GameState_InventoryScreen     = 14,
        GameState_MapScreen           = 15,
        GameState_SaveScreen          = 16,
        GameState_DebugMoviePlayer    = 17,
        GameState_OptionScreen        = 18,
        GameState_LoadStatusScreen    = 19,
        GameState_LoadMapScreen       = 20,
        GameState_Unk15               = 21,
        GameState_Unk16               = 22, /** Removed debug menu? Doesn't exist in function array, but `DebugMoviePlayer` state tries to switch to it. */

        GameState_Hack                = NO_VALUE // @hack Force enum to be treated as s32.
    };

    /** @brief State IDs used by `GameState_InGame`.
     *
     * The values are used as indices into the 0x800A9A2C function array.
     */
    enum e_SysState
    {
        SysState_Gameplay       = 0,
        SysState_OptionsMenu    = 1,
        SysState_StatusMenu     = 2,
        SysState_MapScreen      = 3,
        SysState_Fmv            = 4,
        SysState_LoadOverlay    = 5,
        SysState_LoadRoom       = 6, // Alternative: SysState_LoadArea
        SysState_ReadMessage    = 7,
        SysState_SaveMenu0      = 8,
        SysState_SaveMenu1      = 9,
        SysState_EventCallback  = 10,
        SysState_EventSetFlag   = 11,
        SysState_EventPlaySound = 12,
        SysState_GameOver       = 13,
        SysState_GamePaused     = 14,
        SysState_Invalid        = 15 /** Used by `Event_Update` to signal that SysState shouldn't be updated. */
    };

    /** @brief Inventory command IDs. */
    enum e_InventoryCmdId
    {
        InventoryCmdId_UseHealth     = 0, /** Text is "Use", but this one is used exclusively on health items. */
        InventoryCmdId_Use           = 1,
        InventoryCmdId_Equip         = 2,
        InventoryCmdId_Unequip       = 3,
        InventoryCmdId_EquipReload   = 4,
        InventoryCmdId_UnequipReload = 5,
        InventoryCmdId_OnOff         = 6,
        InventoryCmdId_Reload        = 7,
        InventoryCmdId_Look          = 8,
        InventoryCmdId_UseLook       = 9,
        InventoryCmdId_Unk10         = 10,
        InventoryCmdId_Unk11         = 11 // Flashlight in daytime?
    };

    /** @brief Inventory item groups. Every 32nd item ID is treated as a separate group by some code. */
    typedef enum _InventoryItemGroup
    {
        InventoryItemGroup_None          = 0,
        InventoryItemGroup_HealthItems   = 1,
        InventoryItemGroup_Keys          = 2,
        InventoryItemGroup_PuzzleItems   = 3,
        InventoryItemGroup_MeleeWeapons  = 4,
        InventoryItemGroup_GunWeapons    = 5,
        InventoryItemGroup_GunAmmo       = 6,
        InventoryItemGroup_PortableItems = 7
    } e_InventoryItemGroup;

    /** @brief Inventory item IDs. */
    enum e_InventoryItemId
    {
        // Group 0 (None)
        InventoryItemId_Empty                 = NO_VALUE,
        InventoryItemId_Unequipped            = 0,

        // Group 1 (Health Items)
        InventoryItemId_HealthDrink           = 32,
        InventoryItemId_FirstAidKit           = 33,
        InventoryItemId_Ampoule               = 34,

        // Group 2 (Keys)
        InventoryItemId_LobbyKey              = 64,
        InventoryItemId_HouseKey              = 65,
        InventoryItemId_KeyOfLion             = 66,
        InventoryItemId_KeyOfWoodman          = 67,
        InventoryItemId_KeyOfScarecrow        = 68,
        InventoryItemId_LibraryReserveKey     = 69,
        InventoryItemId_ClassroomKey          = 70,
        InventoryItemId_KGordonKey            = 71,
        InventoryItemId_DrawbridgeKey         = 72,
        InventoryItemId_BasementKey           = 73,
        InventoryItemId_BasementStoreroomKey  = 74,
        InventoryItemId_ExaminationRoomKey    = 75,
        InventoryItemId_AntiqueShopKey        = 76,
        InventoryItemId_SewerKey              = 77,
        InventoryItemId_KeyOfOphiel           = 78,
        InventoryItemId_KeyOfHagith           = 79,
        InventoryItemId_KeyOfPhaleg           = 80,
        InventoryItemId_KeyOfBethor           = 81,
        InventoryItemId_KeyOfAratron          = 82,
        InventoryItemId_NoteToSchool          = 83,
        InventoryItemId_NoteDoghouse          = 84,
        InventoryItemId_PictureCard           = 85,

        InventoryItemId_SewerExitKey          = 87,
        InventoryItemId_ChannelingStone       = 88,

        // Group 3 (Puzzle Items)
        InventoryItemId_Chemical              = 96,
        InventoryItemId_GoldMedallion         = 97,
        InventoryItemId_SilverMedallion       = 98,
        InventoryItemId_RubberBall            = 99,
        InventoryItemId_Flauros               = 100,
        InventoryItemId_PlasticBottle         = 101,
        InventoryItemId_UnknownLiquid         = 102,
        InventoryItemId_PlateOfTurtle         = 103,
        InventoryItemId_PlateOfHatter         = 104,
        InventoryItemId_PlateOfCat            = 105,
        InventoryItemId_PlateOfQueen          = 106,
        InventoryItemId_BloodPack             = 107,
        InventoryItemId_DisinfectingAlcohol   = 108,
        InventoryItemId_Lighter               = 109,
        InventoryItemId_VideoTape             = 110,

        InventoryItemId_KaufmannKey           = 112,
        InventoryItemId_Receipt               = 113,
        InventoryItemId_SafeKey               = 114,
        InventoryItemId_Magnet                = 115,
        InventoryItemId_MotorcycleKey         = 116,
        InventoryItemId_BirdCageKey           = 117,
        InventoryItemId_Pliers                = 118,
        InventoryItemId_Screwdriver           = 119,
        InventoryItemId_Camera                = 120,
        InventoryItemId_RingOfContract        = 121,
        InventoryItemId_StoneOfTime           = 122,
        InventoryItemId_AmuletOfSolomon       = 123,
        InventoryItemId_CrestOfMercury        = 124,
        InventoryItemId_Ankh                  = 125,
        InventoryItemId_DaggerOfMelchior      = 126,
        InventoryItemId_DiskOfOuroboros       = 127,

        // Group 4 (Melee Weapons)
        InventoryItemId_KitchenKnife          = 128,
        InventoryItemId_SteelPipe             = 129,
        InventoryItemId_RockDrill             = 130,

        InventoryItemId_Hammer                = 132,
        InventoryItemId_Chainsaw              = 133,
        InventoryItemId_Katana                = 134,
        InventoryItemId_Axe                   = 135,

        // Group 5 (Guns)
        InventoryItemId_Handgun               = 160,
        InventoryItemId_HuntingRifle          = 161,
        InventoryItemId_Shotgun               = 162,
        InventoryItemId_HyperBlaster          = 163,

        InventoryItemId_CutscenePhone         = 164,
        InventoryItemId_CutsceneFlauros       = 165,
        InventoryItemId_CutsceneAglaophotis   = 166,
        InventoryItemId_CutscenePlasticBottle = 167,
        InventoryItemId_CutsceneBaby          = 168,
        InventoryItemId_CutsceneBloodPack     = 169,

        // Group 6 (Gun Ammo)
        InventoryItemId_HandgunBullets        = 192,
        InventoryItemId_RifleShells           = 193,
        InventoryItemId_ShotgunShells         = 194,

        // Group 7 (Portable Items)
        InventoryItemId_Flashlight            = 224,
        InventoryItemId_PocketRadio           = 225,
        InventoryItemId_GasolineTank          = 226
    };

    /** @brief Common pickup item IDs. */
    enum e_CommonPickupItemId
    {
        CommonPickupItemId_FirstAidKit    = 0,
        CommonPickupItemId_HealthDrink    = 1,
        CommonPickupItemId_Ampoule        = 2,
        CommonPickupItemId_HandgunBullets = 3,
        CommonPickupItemId_RifleShells    = 4,
        CommonPickupItemId_ShotgunShells  = 5
    };

    /** @brief Attack input types. Packed into a weapon attack using `WEAPON_ATTACK`. */
    enum e_AttackInputType
    {
        AttackInputType_Tap      = 0,
        AttackInputType_Hold     = 1,
        AttackInputType_Multitap = 2
    };

    /** @brief Equipped weapon IDs. Derivative of `e_InventoryItemId`.
     *
     * TODO: Maybe just "Weapon ID", "equipable item ID", "[something else] item ID"?
     */
    enum e_EquippedWeaponId
    {
        EquippedWeaponId_KitchenKnife   = 0,
        EquippedWeaponId_SteelPipe      = 1,
        EquippedWeaponId_RockDrill      = 2,
        EquippedWeaponId_Unk3           = 3,
        EquippedWeaponId_Hammer         = 4,
        EquippedWeaponId_Chainsaw       = 5,
        EquippedWeaponId_Katana         = 6,
        EquippedWeaponId_Axe            = 7,
        EquippedWeaponId_Kick           = 8,
        EquippedWeaponId_Stomp          = 9,

        EquippedWeaponId_Unk31          = 31, // Larval Stalker attack.
        EquippedWeaponId_Handgun        = 32,
        EquippedWeaponId_HuntingRifle   = 33,
        EquippedWeaponId_Shotgun        = 34,
        EquippedWeaponId_HyperBlaster   = 35,

        EquippedWeaponId_Unk37          = 37, // Split Head attack.

        EquippedWeaponId_Unk44          = 44, // } Hanged Scratcher attack.
        EquippedWeaponId_Unk45          = 45, // }

        EquippedWeaponId_Unk48          = 48, // } Stalker attack.
        EquippedWeaponId_Unk49          = 49, // }

        EquippedWeaponId_Unk56          = 56, // Puppet Nurse attack.

        EquippedWeaponId_Unk59          = 59, // Float Stinger attack.

        EquippedWeaponId_Unk61          = 61, // Twinfeeler attack.

        EquippedWeaponId_Unk63          = 63, // Cybil or Monster Cybil attack.

        EquippedWeaponId_HandgunBullets = 64, // Monster Cybil attack?
        EquippedWeaponId_RifleShells    = 65, // Monster Cybil attack?
        EquippedWeaponId_ShotgunShells  = 66, // Monster Cybil attack?

        EquippedWeaponId_Unk69          = 69, // Bloodsucker attack.
        EquippedWeaponId_Unk70          = 70, // Kaufmann attack on Dahlia?

        EquippedWeaponId_Flashlight     = 96,
        EquippedWeaponId_PocketRadio    = 97,
        EquippedWeaponId_GasolineTank   = 98
    };

    /** @brief Player control flags. */
    enum e_PlayerFlags
    {
        PlayerFlag_None           = 0,
        PlayerFlag_Unk0           = 1 << 0,
        PlayerFlag_Shooting       = 1 << 1,
        PlayerFlag_Unk2           = 1 << 2,
        PlayerFlag_WallStopRight  = 1 << 3, /** Use right leg for wall stop. */
        PlayerFlag_Unk4           = 1 << 4,
        PlayerFlag_Unk5           = 1 << 5, // `PlayerFlag_MoveBackward`?
        PlayerFlag_Unk6           = 1 << 6,
        PlayerFlag_Unk7           = 1 << 7,
        PlayerFlag_Unk8           = 1 << 8,
        PlayerFlag_Unk9           = 1 << 9,
        PlayerFlag_Unk10          = 1 << 10, // `PlayerFlag_MeleeAttack`?
        PlayerFlag_Unk11          = 1 << 11, // `PlayerFlag_GunAttack`?
        PlayerFlag_Unk12          = 1 << 12,
        PlayerFlag_SfxActive      = 1 << 13,
        PlayerFlag_DamageReceived = 1 << 14,
        PlayerFlag_Moving         = 1 << 15,
        PlayerFlag_Unk16          = 1 << 16,
        PlayerFlag_Unk17          = 1 << 17,
        PlayerFlag_Unk18          = 1 << 18,
        PlayerFlag_Unk19          = 1 << 19,
        PlayerFlag_Unk20          = 1 << 20,

        PlayerFlag_Unk28          = 1 << 28,
        PlayerFlag_Unk29          = 1 << 29,
        PlayerFlag_Unk30          = 1 << 30,
        PlayerFlag_Unk31          = 1 << 31
    };

    /** @brief Character IDs. The `CHARA_FILE_INFOS` array associates each character ID with asset files. */
    enum e_CharaId
    {
        Chara_None             = 0,
        Chara_Harry            = 1,
        Chara_AirScreamer      = 2,
        Chara_NightFlutter     = 3,
        Chara_Groaner          = 4,
        Chara_Wormhead         = 5,
        Chara_LarvalStalker    = 6,
        Chara_Stalker          = 7,
        Chara_GreyChild        = 8,
        Chara_Mumbler          = 9,
        Chara_HangedScratcher  = 10,
        Chara_Creeper          = 11,
        Chara_Romper           = 12,
        Chara_Chicken          = 13, /** @unused */
        Chara_SplitHead        = 14,
        Chara_Floatstinger     = 15,
        Chara_PuppetNurse      = 16,
        Chara_DummyNurse       = 17, /** Uses dummy anim file without model/texture, but the same update funcptr as `Chara_PuppetNurse`. */
        Chara_PuppetDoctor     = 18,
        Chara_DummyDoctor      = 19, /** Uses dummy anim file without model/texture, but the same update funcptr as `Chara_PuppetDoctor`. */
        Chara_Twinfeeler       = 20,
        Chara_Bloodsucker      = 21,
        Chara_Incubus          = 22,
        Chara_Unknown23        = 23,
        Chara_MonsterCybil     = 24,
        Chara_LockerDeadBody   = 25,
        Chara_Cybil            = 26,
        Chara_EndingCybil      = 27,
        Chara_Cheryl           = 28,
        Chara_Cat              = 29,
        Chara_Dahlia           = 30,
        Chara_EndingDahlia     = 31,
        Chara_Lisa             = 32,
        Chara_BloodyLisa       = 33,
        Chara_Alessa           = 34,
        Chara_GhostChildAlessa = 35,
        Chara_Incubator        = 36,
        Chara_BloodyIncubator  = 37,
        Chara_Kaufmann         = 38,
        Chara_EndingKaufmann   = 39,
        Chara_Flauros          = 40,
        Chara_LittleIncubus    = 41,
        Chara_GhostDoctor      = 42,
        Chara_Parasite         = 43,
        Chara_Padlock          = 44,

        Chara_Count,

        Chara_Hack = NO_VALUE, // @hack Force enum to be treated as `s32`.
    };

    /** @brief Game difficulties. */
    enum e_GameDifficulty
    {
        GameDifficulty_Easy   = -1,
        GameDifficulty_Normal = 0,
        GameDifficulty_Hard   = 1
    };

    /** @brief Event trigger types. */
    enum e_TriggerType
    {
        TriggerType_EndOfArray     = NO_VALUE,
        TriggerType_None           = 0, /** Skips trigger/activation type checks. Always activates if required event flags are set and skips processing later events until flags deactivate it. */
        TriggerType_TouchAabb      = 1, /* Player has collided with an AABB. */
        TriggerType_TouchFacing    = 2, /* Player collided with a trigger is facing toward it. */
        TriggerType_TouchObbFacing = 3, /* Player collided with an OBB and is facing toward it. */
        TriggerType_TouchObb       = 4, /* Player collided with an OBB. No facing requirement. */
    };

    /** @brief Event triger activation types. */
    enum e_TriggerActivationType
    {
        TriggerActivationType_None      = 0, /** No activation conditions other than event flag/trigger checks. */
        TriggerActivationType_Exclusive = 1, /** Prevents other events from being triggered while the event is active. */
        TriggerActivationType_Button    = 2, /** Requires a button press. */
        TriggerActivationType_Item      = 3, /** Requires an inventory item. */
    };

    /** Some events indicate specific cutscenes behavior via flags. */
    enum e_EventDataUnkState
    {
        EventParamUnkState_None = 0,
        EventParamUnkState_0    = 1 << 0, /** Possible name: `EventParamUnkState_UnfreezeWorld`. Used to freeze/unfreeze the game world. */
        EventParamUnkState_1    = 1 << 1, /** Triggers cutscenes that go to the in-game mode. */
        EventParamUnkState_2    = 1 << 2
    };

    typedef union
    {
        u32 rawData_0;
        struct
        {
            s8 rightX;
            s8 rightY;
            s8 leftX;
            s8 leftY;
        } sticks_0; // Normalized range: `[-128, 127]`.
    } s_AnalogSticks;

    struct s_AnalogController
    {
        u8  status;
        u8  received_bytes : 4; /** Number of bytes received / 2. */
        u8  terminal_type  : 4; /** `e_PadTerminalType` */
        u16 digitalButtons;
        u8  rightX;
        u8  rightY;
        u8  leftX;
        u8  leftY;
    };

    struct s_ControllerData
    {
        s_AnalogController analogController_0;
        s32                pulseTicks_8;
        int                btnsHeld_C;       /** `e_ControllerFlags` */
        int                btnsClicked_10;   /** `e_ControllerFlags` */
        int                btnsReleased_14;  /** `e_ControllerFlags` */
        int                btnsPulsed_18;    /** `e_ControllerFlags` */
        int                btnsPulsedGui_1C; /** `e_ControllerFlags` */
        s_AnalogSticks     sticks_20;
        s_AnalogSticks     sticks_24;
        s32                field_28;
    };

    /** @brief Controller key bindings for input actions. Contains bitfield of button presses assigned to each action.
     *
     * Bitfields only contain buttons. Analog directions and D-Pad aren't included.
     */
    struct s_ControllerConfig
    {
        u16 enter;
        u16 cance;
        u16 skip;
        u16 action;
        u16 aim;
        u16 light;
        u16 run;
        u16 view;
        u16 stepLeft;
        u16 stepRight;
        u16 pause;
        u16 item;
        u16 map;
        u16 option;
    };

    /** @brief Inventory item entry. */
    struct s_InventoryItem
    {
        u8 id_0;      /** `InventoryItemId` */
        u8 count_1;
        u8 command_2; /** `InventoryCmdId` */
        u8 field_3;   // Some sort of index?
    };

    /** @brief Special inventory item toggle flags. */
    enum e_ItemToggleFlags
    {
        ItemToggleFlag_RadioOn       = 1 << 0,
        ItemToggleFlag_FlashlightOff = 1 << 1
    };

    /** @brief Savegame data. */
    typedef struct _Savegame
    {
        s_InventoryItem items_0[INVENTORY_ITEM_COUNT_MAX];
        s8              field_A0;
        s8              unused_A1[3]; /** @unused */
        s8              mapIdx;       /** `e_MapIdx` Index to overlay `.BIN` files. */
        s8              mapRoomIdx;   /** Index to local map geometry `.IPD` files. */
        s16             savegameCount;
        s8              locationId;                  /** `e_SaveLocationId` */
        u8              paperMapIdx;                 /** `e_PaperMapIdx` | Index of the paper map displayed when opening the map screen. */
        u8              equippedWeapon;              /** `e_InventoryItemId` | Affects the visible player weapon model. */
        u8              inventorySlotCount;                /** Item slots. */
        u32             itemToggleFlags;             /** `e_ItemToggleFlags` */
        s32             ovlEnemyStates[Chara_Count]; /** Flags indicating the enemy states in a given overlay.
                                                     * By default, they are all set to 1. As soon as the player fully kills them,
                                                     * they are set to 0 based on a currently unknown index value.
                                                     */
        s32             hasMapsFlags;                // See Sparagas' `HasMapsFlags` struct for details of every bit.
        u32             eventFlags[27];              // Can be accessed through `Savegame_EventFlagGet` / `Savegame_EventFlagSet`, only tested a few, but seems all are related to events and pick-up flags
                                                     // grouped by location and not item types.
        s32             mapMarkingFlags[25];         // See Sparagas' `MapMarkingsFlags` struct for details of every bit.
        q19_12          healthSaturation;            /** Range: [0, 300]. Ampoules give extra stored health. If the player loses health, it will be slowly restored. */
        s16             pickedUpItemCount;
        s8              unused_23E;         /** @unused */
        u8              inventoryItemFlags; /** `e_InventoryItemFlags` */
        q19_12          playerHealth;       /** Default: `Q12(100.0f)` */
        q19_12          playerPositionX;
        q3_12           playerRotationY;  /** Range [0, 0.999755859375], positive Z: 0, clockwise rotation. It can be multiplied by 360 to get degrees. */
        u8              clearGameCount;   /** Range [0, 99] */
        u8              clearGameEndings; /** `e_GameEndingFlags` */
        q19_12          playerPositionZ;
        q20_12          gameplayTimer;
        q20_12          runDistance;
        q20_12          walkDistance;
        u8              isNextFearMode           : 1; /** Makes savegame entry text gold. */
        u8              add290Hours              : 2; /** Adds 290 hours per 1 bit, i.e. 290, 580, 870. */
        u8              pickedUpSpecialItemCount : 5; /** Red/None: 0?, Yellow: 8, Green: 16, @unused Rainbow: 24. */
                                                       /** Sparagas' investigations indicate this variable should be
                                                        * two different variables. However, splitting it causes minor
                                                        * mismatches in some functions.
                                                        *
                                                        * The first 3 bits indicate the number of special items the
                                                        * player has picked up, and the last 2 bits indicate the color of the Hyper
                                                        * Blaster beam.
                                                        *
                                                        * Belek666 suggests that some functions specifically access this field as 5 bits.
                                                        *
                                                        * The the odd access results in a bug where the results screen will count more collected
                                                        * special items than normal by additionally reading one of the two bits
                                                        * for the Hyper Blaster beam color.
                                                        */
        u8              meleeKillCount;
        u8              meleeKillCountB; // Can't be packed if used as `u16`.
        u8              rangedKillCount;
        u32             field_260          : 28;
        s32             gameDifficulty : 4;  /** `e_GameDifficulty` */
        u16             firedShotCount;      /** Missed shot count = firedShotCount - (closeRangeShotCount + midRangeShotCount + longRangeShotCount). */
        u16             closeRangeShotCount; /** Only hits counted. */
        u16             midRangeShotCount;   /** Only hits counted. */
        u16             longRangeShotCount;  /** Only hits counted. */
        u16             field_26C;
        u16             field_26E; // Related to enemy kills.
        u16             field_270;
        u16             field_272;
        u16             field_274;
        u16             field_276;
        u16             field_278;
        s8              field_27A; // Flags.
        u8              continueCount;
    } s_Savegame;

    /** TODO: Known as `Trigger` in SilentHillMapExaminer: https://github.com/ItEndsWithTens/SilentHillMapExaminer/blob/master/src/SHME.ExternalTool.Guts/Trigger.cs */
    struct s_EventData
    {
        s16 requiredEventFlag;
        s16 disabledEventFlag;
        s8  triggerType    : 4;  /** `e_TriggerType` */
        u8  activationType : 4;  /** `e_TriggerActivationType` */
        u8  pointOfInterestIdx;  /** Index into `g_MapOverlayHeader.mapPoints`. */
        u8  requiredItemId;      /** `e_InventoryItemId` that player must use from item screen. */
        u32 sysState        : 5; /** `e_SysState` used by the event. */
        u32 eventParam      : 8; /** Can be an ID of a `MapMsg`, sound effect, index into `mapEventFuncs`, or index into `mapPoints` for `areaLoad` events. */
        u32 flags_8_13      : 6; /** `e_EventDataUnkState` */
        u32 sfxPairIdx_8_19 : 5;
        u32 field_8_24      : 1;
        u32 mapOverlayIdx   : 6;
        u32 field_8_31      : 1;
    };

    /** @brief User options configuration. */
    typedef struct _SaveUserConfig
    {
        s_ControllerConfig controllerConfig_0;
        s8                 optScreenPosX_1C;          /** Range: [-11, 11], default: 0. */
        s8                 optScreenPosY_1D;          /** Range: [-8, 8], default: 0. */
        bool               optSoundType_1E;           /** | Stereo: `false`, Monaural: `true`, default: Stereo. */
        u8                 optVolumeBgm_1F;           /** Range: [0, 128] with steps of 8, default: 16. */
        u8                 optVolumeSe_20;            /** Range: [0, 128] with steps of 8, default: 16. */
        bool               optVibrationEnabled_21;    /** Off: 0, On: 128, default: On. */
        u8                 optBrightness_22;          /** Range: [0, 7], default: 3. */
        bool               optExtraWeaponCtrl_23;     /** Switch: `false`, Press: `true`, default: Press. */
        u8                 optExtraBloodColor_24;     /** `e_BloodColor` | Default: Normal. */
        bool               optAutoLoad_25;            /** Off: `false`, On: `true`, default: Off. */
        u8                 optExtraOptionsEnabled_27; /** Holds unlocked option flags. */
        bool               optExtraViewCtrl_28;       /** Normal: `false`, Reverse: `true`, default: Normal. */
        bool               optExtraViewMode_29;       /** Normal: `false`, Self View: `true`, default: Normal. */
        bool               optExtraRetreatTurn_2A;    /** Normal: `false`, Reverse: `true`, default: Normal. */
        bool               optExtraWalkRunCtrl_2B;    /** Normal: `false`, Reverse: `true`, default: Normal. */
        bool               optExtraAutoAiming_2C;     /** On: `false`, Off: `true`, default: On. */
        bool               optExtraBulletAdjust_2D;   /** x1-x6: Range [0, 5], default: x1. */
        u16                seenGameOverTips_2E[1];    /** Bitfield tracking seen game-over tips. Each bit corresponds to a tip index (0–14), set bits indicate seen tips. Resets after picking all 15. */
        s8                 unk_30[4];
        u32                palLanguageId_34;
    } s_SaveUserConfig;

    /** @brief Game workspace. Stores miscellaneous gameplay-related data. */
    typedef struct _GameWork
    {
        s_SaveUserConfig   config;
        s_ControllerData   controllers[CONTROLLER_COUNT_MAX];
        s_Savegame         autosave;
        s_Savegame         savegame;
        u16                gsScreenWidth;
        u16                gsScreenHeightx;
        s_PrimColor        background2dColor;
        e_GameState        gameStatePrev;
        e_GameState        gameState;
        s32                gameStateSteps[3]; /** Sub-state steps used by the current `gameState`. Can be other state IDs or data. */
        s8                 unk_5A4[4];        // Padding?
        s32                field_5A8;
        s32                field_5AC;
        // 1 byte of padding.
        s8                 mapAnimIdx;
        s8                 bgmIdx;            /** `BgmTrackIdx` | Currently player background music track. */
        s8                 ambientIdx;        /** Index of `g_AmbientVabTaskLoadCmds`. */
        s_AnalogController rawController;
        s8                 unk_5BC[28];       // @unused Debug data?
    } s_GameWork;

    /** @brief Constant character animation info passed to `Anim_Update` functions.
     * Defines which `Anim_Update` function is to be called.
     */
    struct s_AnimInfo
    {
        void (*playbackFunc)(struct _Model* model, struct _AnmHeader* anmHdr, GsCOORDINATE2* boneCoords, struct _AnimInfo* animInfo);
        u8   status;                      /** Packed anim status. Init base? See `s_ModelAnimData::status`. */
        bool hasVariableDuration;         /** Use `duration.variableFunc`: `true`, Use `duration.constant`: `false`. */
        u8   linkStatus;                  /** Packed anim status link target. See `s_ModelAnim::status`. */
        union
        {
            q19_12 constant;              /** Constant duration at 30 FPS. */
            q19_12 (*variableFunc)(void); /** Variable duration at 30 FPS via a function. Allows animations to be sped up or slowed down. */
        } duration;
        s16 startKeyframeIdx;             /** Start keyframe index. Sometimes `NO_VALUE`, unknown why. */
        s16 endKeyframeIdx;               /** End keyframe index. */
    };

    /** @brief Character model animation. */
    typedef struct _ModelAnim
    {
        u8          status;             /** Is active: bit 0, Anim index: bits 1-7. */
        u8          mapAnimStatusStart; /** Start packed anim status of map-specific anim infos. Only used for Harry. */
        u16         flags;              /** `e_AnimFlags` */
        q19_12      time;               /** Time on timeline. */
        s16         keyframeIdx;        /** Active keyframe index. */
        q3_12       alpha;              /** Keyframe progress alpha. */
        s_AnimInfo* baseAnimInfos;      /** Anim infos. For Harry, used for base anims. */
        s_AnimInfo* mapAnimInfos;       /** Map-specific anim infos. Only used for Harry. */
    } s_ModelAnim;

    /** @brief Character model. */
    struct s_Model
    {
        s8          charaId;      /** `e_CharaId` */
        u8          paletteIdx;   /** Changes the texture palette index for this model. */
        u8          controlState; /** Active character control state. */
        u8          stateStep;    /** Step for the current `controlState`. */ 
                                  // In `s_PlayerExtra` always 1, set to 0 for 1 tick when anim state appears to change.
                                  // Used differently in player's `s_SubCharacter`. 0: anim transitioning(?), bit 1: animated, bit 2: turning.
                                  // Sometimes holds actual anim index?
        s_ModelAnim anim;
    };

    // TODO: Unsure if this struct is puppet doctor specific or shared with all characterss. Pointer gets set at puppetDoc+0x124.
    typedef struct
    {
        q19_12      health_0;
        s32         field_4;
        s32         field_8;
        s32         field_C;
        s8          unk_10[8];
        s32         field_18;
        s32         idx_1C;
        s32         field_20;
        s_AnimInfo* animInfo_24;
        s8          unk_28[4];
        q19_12      field_2C;
        s8          unk_30[4];
    } s_800D5710;

    // Collision-related.
    typedef struct
    {
        VECTOR3 position;
        s16     field_C;
        s16     field_E;
        s16     field_10;
        s8      collisionState; /** `e_CharaCollisionState` */
        u8      field_13;
    } s_func_8006CF18;

    struct s_CharaDamage
    {
        VECTOR3 position;
        q19_12  amount;
    };

    typedef union
    {
        s32 val32;
        s16 val16[2];
        s8  val8[4];
    } u_Property;

    /** @brief Temporary struct. */
    typedef struct _PropsDummy
    {
        u_Property properties_E8[16];
    } s_PropsDummy;

    // TODO: Re-offset `s_PropsPlayer` / `s_PropsNpc`.
    // Probably easier to do that after it's merged with rest of code.
    typedef struct _PropsPlayer
    {
        q19_12        afkTimer; // Increments every tick for 10 seconds before AFK anim starts.
        q19_12        positionY_EC;
        q19_12        field_F0;
        q19_12        field_F4; // Angle related to X axis flex rotation.
        q19_12        runTimer_F8;
        q19_12        exhaustionTimer;
        q19_12        field_100;    // Angle?
        q19_12        field_104;    // Distance?
        q19_12        runTimer_108;
        u8            field_10C;    // Player SFX pitch?
        u8            field_10D;
        q19_12        timer_110; // Increases when `flags & CharaFlag_3` is set, reset when reaches `D_800C45EC`.
        q19_12        gasWeaponPowerTimer; // Timer for the rock drill and chainsaw power.
        s16           field_118;
        e_PlayerFlags flags_11C;
        q3_12         quickTurnHeadingAngle; /** Target quick turn heading angle. */
        q3_12         field_122; // Some sort of X angle for the player. Specially used when aiming an enemy.
        q3_12         headingAngle;
        q3_12         moveDistance_126; // Used to indicate how much the player should move foward. Seems to be squared.
    } s_PropsPlayer;

    // TODO: This may be a puppet doctor/nurse specific struct, need to compare with other NPCs.
    typedef struct _PropsNpc
    {
        VECTOR3     position_E8; /** Q19.12 */
        s32         field_F4;
        s32         field_F8;
        s32         field_FC;
        s32         field_100;
        s32         field_104;
        s32         field_108;
        s32         field_10C;
        s8          unk_110[4];
        s32         field_114;
        s8          field_118;
        u8          modelVariantIdx;
        s16         field_11A;
        s32         field_11C;
        s16         field_120;
        s16         field_122;
        s_800D5710* field_124;
    } s_PropsNpc;

    /** @brief Air Screamer or Night Flutter character properties. */
    typedef struct _PropsAirScreamer
    {
        u32     field_E8_0 : 4; // `AirScreamerHit_None` step.
        bool    field_E8_4;
        u32     field_E8_8 : 4;
        u32     __pad_E8_C : 20;
        s32     field_EC;
        s16     field_F0; // } Maybe 2D offset like in Creeper properties? Must check.
        s16     field_F2; // }
        s32     field_F4;
        VECTOR3 targetPosition; /** Q19.12 */
        VECTOR3 position_104;   /** Q19.12 | Set to either Air Screamer position with slight offset toward player or player position. */
        VECTOR3 position_110;
        s32     flags; /** `e_AirScreamerFlags` */
        q19_12  timer_120;
        q19_12  groundHeight;
    } s_PropsAirScreamer;

    /** @brief Alessa character properties. */
    typedef struct _PropsAlessa
    {
        s32        controlState; /** `e_AlessaControl` */
        u_Property properties_EC; // Unused?
        s32        field_F0;      // `bool`? If `false`, animation gets updated.
        u_Property properties_F4; // Unused?
        bool       resetControlState;
        s32        field_FC;
        s32        field_100;
        u_Property properties_104;
        u_Property properties_108;
        u_Property properties_10C;
        VECTOR3    field_110;
        s32        flags_11C;
        u_Property properties_120;
        s16        field_124;
        q3_12      moveSpeed_126;
    } s_PropsAlessa;

    /** @brief Bloodsucker character properties. */
    typedef struct _PropsBloodsucker
    {
        q19_12 timer_E8;
        q19_12 timer_EC;
        q19_12 timer_F0;
        q19_12 timer_F4;
        s8     unused_F8[36]; /** @unused */
        s32    flags_118;     /** `e_BloodsuckerFlags` */
    } s_PropsBloodsucker;

    /** @brief Cheryl character properties. */
    // TODO: Needs revision. Copy of Dahlia properties.
    typedef struct _PropsCheryl
    {
        s32        controlState; /** `e_CherylControl` */
        u_Property properties_EC; // Unused?
        s32        field_F0;
        u_Property properties_F4; // Unused?
        bool       resetControlState;
        s32        field_FC;
        s32        field_100;
        u_Property properties_104;
        u_Property properties_108;
        u_Property properties_10C;
        VECTOR3    field_110;
        s32        flags; /** `e_CherylFlags` */
        u_Property properties_120;
        q3_12      moveDistance_124;
        q3_12      moveDistance_126;
    } s_PropsCheryl;

    /** @brief Creeper character properties. */
    typedef struct _PropsCreeper
    {
        u16    flags; /** `e_CreeperFlags` */
        s8     __pad_EA[2];
        q3_12  collisionOffsetX;
        q3_12  collisionOffsetZ;
        q19_12 attackTimer;
        q19_12 targetPositionX;
        q19_12 targetPositionZ;
        q19_12 homePositionX;
        q19_12 homePositionZ;
        q19_12 chirpTimer;
        q3_12  angleToTarget;
        s16    animStatus_10A; // TODO: Purpose unclear.
        q4_12  moveSpeed;
    } s_PropsCreeper;

    /** @brief Dahlia character properties. */
    typedef struct _PropsDahlia
    {
        s32        controlState; /** `e_DahliaControl` */
        u_Property properties_EC;
        u_Property properties_F0;
        u_Property properties_F4;
        bool       resetControlState;
        s32        field_FC;
        s32        field_100;
        u_Property properties_104;
        u_Property properties_108;
        u_Property properties_10C;
        VECTOR3    field_110;
        s32        flags_11C;
        u_Property properties_120;
        s16        field_124;
        q3_12      moveDistance_126;
    } s_PropsDahlia;

    /** @brief Floatstinger character properties. */
    typedef struct _PropsFloatstinger
    {
        s16        flags_E8;
        s8         unk_EA[2];
        s8         unk_EC[2];
        q3_12      field_EE; // Angle.
        q19_12     field_F0;
        s16        field_F4;
        s8         unk_F6[2];
        u16        field_F8;
        u16        field_FA;
        u16        field_FC;
        s8         unk_FE[2];
        s32        field_100;
        u8         field_104;
        s8         unk_105;
        s16        field_106;
        s32        field_108;
        s16        field_10C;
        q3_12      field_10E;
        u_Property properties_110[6];
    } s_PropsFloatstinger;

    /** @brief Groaner character properties. */
    typedef struct _PropsGroaner
    {
        u_Property flags_E8; /** `e_GroanerFlags` TODO: One weird exception where it's accessed as `s32`. */
        q3_12      targetHeadingAngle;
        q3_12      flexAngle;
        q3_12      field_F0;
        q3_12      field_F2;
        q19_12     targetPositionX_F4;
        q19_12     targetPositionZ_F8;
        q3_12      angleToTarget;
        q3_12      field_FE;
        u16        relKeyframeIdx_100;
        s8         unk_102[2];
        q19_12     timer_104;
        u32        field_108;
        q3_12      timer_10C; // SFX timer?
        u8         field_10E; // } Sound states?
        u8         field_10F; // }
        u8         playLeftFootstepSfx;  /** `bool` */
        u8         playRightFootstepSfx; /** `bool` */
        s8         __pad_112[2];
        q3_12      field_114; // Move speed coefficient?
    } e_PropsGroaner;

    /** @brief Hanged Scratcher character properties. */
    typedef struct _PropsHangedScratcher
    {
        s16    flags_E8; /** `e_HangedScratcherFlags` */
        q4_12  timer_EA;
        q3_12  targetHeadingAngle_EC;
        u8     field_EE;
        s8     unk_EF[1];
        q3_12  offsetX_F0; // } Offsets passed to `func_8005CB20`
        q3_12  offsetZ_F2; // }
        q19_12 positionX_F4;
        q19_12 positionZ_F8;
        q3_12  field_FC;
        s8     unk_FE[2];
        q3_12  timer_100;
        u8     field_102;
        u8     field_103;
        s32    field_104;
        q3_12  timer_108;
        s8     unk_10A[2];
        q4_12  radiusMax_10C; // } Used as `Chara_MoveSpeedUpdate` limit param, TODO: rename?
        q4_12  radiusMin_10E; // }
    } s_PropsHangedScratcher;

    /** @brief Incubus or Incubator character properties. TODO: Check which one. */
    typedef struct _PropsIncubus
    {
        q19_12 timer_E8;
        s32    field_EC;
        s32    someState_F0;
        q19_12 bossFightTimer_F4;
        s8     unk_F8[48];
    } s_PropsIncubus;

    /** @brief Kaufmann character properties. TODO: Largely a copy of Dahlia's for now. */
    typedef struct _PropsKaufmann
    {
        /* 0x0   */ s32        controlState; /** `e_KaufmannControl` */
        /* 0xEC  */ u_Property properties_EC;
        /* 0xF0  */ u_Property properties_F0;
        /* 0xF4  */ u_Property properties_F4;
        /* 0xF8  */ bool       resetControlState;
        /* 0xFC  */ s32        field_FC;
        /* 0x100 */ s32        field_100;
        /* 0x104 */ u_Property properties_104;
        /* 0x108 */ u_Property properties_108;
        /* 0x10C */ u_Property properties_10C;
        /* 0x110 */ VECTOR3    field_110;
        /* 0x11C */ s32        flags_11C;
        /* 0x120 */ s32        field_120;
        /* 0x124 */ s16        field_124;
        /* 0x126 */ q3_12      moveSpeed;
    } s_PropsKaufmann;

    /** @brief Larval Stalker character properties. */
    typedef struct _PropsLarvalStalker
    {
        u16        flags_E8; /** `e_LarvalStalkerFlags` */
        u8         field_EA;
        u8         unk_EB;
        q19_12     timer_EC;
        q20_12     timer_F0;
        s16        keyframeIdx_F4; // Relative keyframe?
        s16        keyframeIdx_F8; // Relative keyframe?
        q19_12     targetPositionX;
        q19_12     targetPositionZ;
        q3_12      angle_100;
        q3_12      angle_102;
        q19_12     animTime_104;
        q3_12      angle_108;
        q4_12      timer_10A;
        u_Property field_10C;
        VECTOR3    field_110;
        s32        flags_11C;
        u_Property field_120;
        s16        field_124;
        q3_12      moveDistance_126;
    } s_PropsLarvalStalker;

    /** @brief Monster Cybil character properties. */
    typedef struct _PropsMonsterCybil
    {
        s32    field_E8;
        s16    field_EC;
        s16    field_EE; // Anim index?
        q19_12 field_F0; // Timer.
        s32    field_F4;
        q19_12 field_F8; // Timer.
        s8     unk_FC[2];
        q3_12  field_FE; // Angle?
        q19_12 field_100;
        s16    field_104;
        q3_12  field_106; // Angle.
        s32    field_108;
        q19_12 field_10C; // Timer.
        q19_12 field_110; // Distance?
        u8     field_114;
        u8     field_115;
        u8     field_116;
        s8     unk_117;
        s16    field_118;
        q3_12  field_11A; // Angle?
        s16    field_11C;
        s16    field_11E;
        s16    field_120;
        s16    field_122;
        s8     unk_124[2];
        s16    field_126;
    } s_PropsMonsterCybil;

    /** @brief Puppet Nurse or Puppet Doctor character properties. */
    typedef struct _PropsPuppetNurse
    {
        VECTOR3       position_E8; /** Q19.12 */
        s_CharaDamage damage;
        q19_12        field_104;
        s32           field_108;
        s32           field_10C;
        q19_12        moveSpeed;
        s32           field_114;
        u8            field_118;
        u8            modelVariantIdx;
        u16           field_11A;
        q3_12         field_11C; // Angle.
        s16           field_11E;
        s16           field_120;
        u16           flags_122; /** `e_PuppetNurseFlags` */
        s_800D5710*   field_124;
    } s_PropsPuppetNurse;

    /** @brief Romper character properties. */
    typedef struct _PropsRomper
    {
        s32    flags; /** `e_RomperFlags` */
        q3_12  angle_EC; // Target heading angle?
        s16    field_EE;
        q3_12  field_F0; // Move speed accumulation for this tick.
        q3_12  rotationY_F2;
        q19_12 field_F4; // Relative anim time?
        q3_12  movementOffsetX; // } Move offset?
        q3_12  movementOffsetZ; // }
        q19_12 targetPositionX_FC;
        q19_12 targetPositionZ_100;
        s32    field_104;
        q19_12 positionX_108;
        u8     field_10C; // Relative keyframe index?
        s8     unk_10D;
        u16    field_10E;
        q19_12 positionZ_110;
        u8     field_114;
        u8     field_115;
        q3_12  field_116;
        q3_12  timer_118;
        u8     field_11A;
        s8     unk_11B;
        q3_12  timer_11C;
        s8     unk_11E[2];
        q19_12 distance_120;
        q19_12 field_124; // Move speed step?
    } s_PropsRomper;

    /** @brief Split Head character properties. */
    typedef struct _PropsSplitHead
    {
        u16     flags; /** `e_SplitHeadFlags` */
        u8      field_EA;
        s8      unk_EB;
        q3_12   angle_EC;
        q3_12   field_EE;
        q4_12   angle_F0;
        q4_12   timer_F2;
        q4_12   timer_F4;
        s8      unk_F8[2];
        q19_12  animTime_F8;
        s32     field_FC;
        s32     field_100;
        s8      unk_104[4];
        u8      field_108[4];
        q19_12  field_10C;
        VECTOR3 field_110;
        s32     flags_11C; /** `e_SplitHeadFlags` */
        s8      unk_120[4];
        s16     field_124;
        q3_12   moveDistance_126;
    } s_PropsSplitHead;

    /** @brief Stalker character properties. */
    typedef struct _PropsStalker
    {
        s16    flags; /** `e_StalkerFlags` */
        q3_12  offset_EC;
        q3_12  offset_EE;
        q19_12 targetPositionX;
        q19_12 targetPositionZ;
        q19_12 timer_F8;
        s16    keyframeIdx_FC;    // Or anim status?? Seems to be used as both.
        s16    relKeyframeIdx_FE; // Unsure.
        q3_12  targetHeadingAngle;
        s16    sfxId_102;
        q19_12 relAnimTime_104;
        q4_12  timer_108;
        u8     field_10A;
        q19_12 timer_10C;
        q19_12 health_110;
        q3_12  angle_114;
        q4_12  timer_116;
    } s_PropsStalker;

    /** @brief Twinfeeler character properties. */
    typedef struct _PropsTwinfeeler
    {
        // TODO: Weird `field_E8` access.
        u_Property    field_E8;
        //q3_12         sfxTimer_E8;
        //q4_12         field_EA;
        s_CharaDamage damage;
        q19_12        digTimer;
        q19_12        spawnPositionX; /** @unused */
        q19_12        spawnPositionZ; /** @unused */
        q19_12        prevMoveSpeed;
        q19_12        accumulatedDamage;
        s16           field_110; /** @unused */
        s8            __pad_112[2];
        u32           flags;     /** `e_TwinfeelerFlags` */
        u16           field_118; /** `bool` */
        s8            __pad_11C[2]; // TODO: Should be `s32 prevHealth`.
        s32           field_120; /** @unused */
        s8            __pad_124[8];
    } s_PropsTwinfeeler;

    typedef struct
    {
        s16     field_0; // Something dependent on `CharaFlag_7`.
        u8      field_2; // In player: packed weapon attack. See `WEAPON_ATTACK`.
                         // This is not the same as `attackReceived`, as this value only resets when player is aiming.
                         // In NPCs: Indicates attack performed on player.
        u8      field_3;
        u8      field_4;
        s8      unk_5[3];
        s32     field_8;  // } Fields used by `func_8008A3E0`. Types guessed for now.
        s16     field_C;  // } Angle?
        s16     field_E;  // } Angle?
        s16     field_10; // }
        s16     field_12; // }
        s32     field_14; // }
        VECTOR3 field_18;
        VECTOR3 field_24[3];
        VECTOR3 field_48[3];
    } s_SubCharacter_44;

    /** @brief Character collision box. */
    typedef struct _SubCharacter_C8
    {
        q3_12 bottom;
        q3_12 top;
        q3_12 height;
        q3_12 offsetY;
        q3_12 field_8; // X extent?? Always negative, but why?
        q3_12 field_A; // Z extent??
    } s_CharaBox;

    /** @brief Character collision cylinder. */
    typedef struct s_CharaCylinder
    {
        q3_12 radius_0;
        q3_12 field_2; // Height?
    } ss_CharaCylinder;

    /** @brief Character collision info for the active animation frame. */
    typedef struct _CharaShapeOffsets
    {
        DVECTOR_XZ box;
        DVECTOR_XZ cylinder;
    } s_CharaShapeOffsets;

    /** @brief Character collision info. */
    typedef struct _CharaCollision
    {
        s_CharaBox          box;
        s_CharaCylinder     cylinder;
        s_CharaShapeOffsets shapeOffsets;   // Translation data?
        u8                  field_E0;       // Related to collision. If the player collides with the only enemy in memory and the enemy is knocked down, this is set to 1.
        s8                  state      : 4; /** `e_CharaCollisionState` */
        u8                  field_E1_4 : 4; // Index for array of `s_func_8006CF18`.
        s_func_8006CF18*    field_E4;
    } s_CharaCollision;

    /** @brief Character info. */
    struct s_SubCharacter
    {
        s_Model           model;    // In player: Manage the half lower part of Harry's body animations (legs and feet).
        VECTOR3           position; /** Q19.12 */
        SVECTOR3          rotation; /** Q3.12 */
        q3_12             angleToTarget;
        SVECTOR3          rotationSpeed;              /** Q3.12 | Rotation speed for `rotation`. */
        q3_12             angleToTargetRotationSpeed; /** Rotation speed for `angleToTarget`. */
        q19_12            fallSpeed;
        q19_12            moveSpeed;
        q3_12             headingAngle;
        s16               flags;          /** `e_CharaFlags` */
        s8                field_40;       // In player: Index of the NPC attacking the player.
                                          // In NPCs: Unknown. `Game_NpcRoomInitSpawn` sugests it indicates the NPC index in `s_Savegame::ovlEnemyStates`.
        s8                attackReceived; // Packed weapon attack indicating what attack has been performed to the character. See `WEAPON_ATTACK`.
        s_SubCharacter_44 field_44;
        q19_12            health;
        s_CharaDamage     damage;
        u16               deathTimer;     // Part of `shBattleInfo` struct in SH2, may use something similar here.
        q3_12             timer_C6;       // Some sort of timer. Written to by `Ai_LarvalStalker_Update`.
        s_CharaCollision  collision;

        s_SubCharacter() { memset(this, 0, sizeof(*this)); }

        union _u
        {
            s_PropsDummy           dummy;
            s_PropsPlayer          player;
            s_PropsNpc             npc;

            s_PropsAirScreamer     airScreamer;
            s_PropsAlessa          alessa;
            s_PropsBloodsucker     bloodsucker;
            s_PropsCheryl          cheryl;
            s_PropsCreeper         creeper;
            s_PropsDahlia          dahlia;
            s_PropsFloatstinger    floatstinger;
            e_PropsGroaner         groaner;
            s_PropsHangedScratcher hangedScratcher;
            s_PropsIncubus         incubus;
            s_PropsKaufmann        kaufmann;
            s_PropsLarvalStalker   larvalStalker;
            s_PropsMonsterCybil    monsterCybil;
            s_PropsPuppetNurse     puppetNurse;
            s_PropsRomper          romper;
            s_PropsSplitHead       splitHead;
            s_PropsStalker         stalker;
            s_PropsTwinfeeler      twinfeeler;

            _u() {}
        } properties;
    };

    /** @brief Extra player character info. */
    struct s_PlayerExtra
    {
        s_Model           model;             /** Manages upper half body's animations (torso, arms, head). */
        s32               disabledAnimBones; /** Bitfield of disabled animation bones. Can be created using the `BITMASK_RANGE` macro. */
        s32               state;             /** `e_PlayerState` */
        s32               upperBodyState;    /** `e_PlayerUpperBodyState` */
        s32               lowerBodyState;    /** `e_PlayerLowerBodyState` */
        e_InventoryItemId lastUsedItem;      /** Holds the last item ID used from inventory when the player is inside an item trigger area. */
    };

    /** @brief Player workspace.
     *
     * Possible original name: `shPlayerWork`.
     */
    struct s_PlayerWork
    {
        s_SubCharacter player;
        s_PlayerExtra  extra;
    };

    /** @brief Player combat info. */
    struct s_PlayerCombat
    {
        VECTOR3 field_0;            // Q19.12 position offset?
        s8      __pad_C[3];
        s8      weaponAttack;       /** Packed weapon attack. See `WEAPON_ATTACK`. */
        u8      currentWeaponAmmo;
        u8      totalWeaponAmmo;
        s8      weaponInventoryIdx; /** Index of the currently equipped weapon in the inventory. */
        bool    isAiming;
    };

    typedef union
    {
        s32 field_0; // Flags?
        struct
        {
            u8 field_0; // Flags.
            u8 field_1;
            u8 field_2;
            u8 unk_3;
        } s_field_0;
    } u_Unk0;

    /** @brief Map effects info. */
    struct s_MapEffectsInfo
    {
        u_Unk0  field_0;
        q3_12   field_4; // Alpha.
        q3_12   field_6; // World tint color intensity.
        q3_12   worldTintR_8;
        q3_12   worldTintG_A;
        q3_12   worldTintB_C;
        u8      field_E; // Fog enabled if not set to 0, `Gfx_FogParametersSet` checks for values 0/1/2/3.
                         // Sets the transparent grey layer overlaid on characters and the enviroment.
        q19_12  fogDistance_10;
        CVECTOR fogColor_14;
        u8      field_18; // `bool`?
        CVECTOR field_19;
        CVECTOR screenTint_1D; // Subtractive screen tint.
        CVECTOR field_21;      // Particle effect related. Only the first value affects snow transparency.
        CVECTOR field_25;
    };

    typedef struct
    {
        s_MapEffectsInfo effectsInfo_0;
        q3_12            flashlightLensFlareIntensity_2C; // Stores flashlight lens flare intensity alpha.
        q3_12            field_2E;                        // Some gamma or brightness layer alpha.
        q19_12           field_30;                        // Stores fog distance.
    } s_StructUnk3;

    typedef struct
    {
        s32             primType_0; /** `e_PrimitiveType` */
        s8*             field_4; /** Points to different types of data depending on `field_0`. */
        s32             field_8; // } Q19.12?
        s32             field_C; // }
        s32             field_10;
        u8              field_14;
        bool            isFlashlightOn_15;
        bool            isFlashlightUnavailable_16;
        q3_12           flashlightIntensity_18;
        u16             field_1A;
        s_StructUnk3    field_1C[2];
        s_StructUnk3    field_84[2];
        s_StructUnk3    field_EC[2];
        s_StructUnk3    field_154;
    } s_SysWork_2388;

    /** @brief Main system workspace. Stores key engine data. */
    struct s_SysWork
    {
        s8             unused_0[8]; /** @unused */
        e_SysState     sysState;
        s32            sysStateSteps[3]; /** Temp data used by current `sysState`. Can be another state ID or other data. */
        bool           isMgsStringSet;   /** Indicates if string have been loaded and is going (or it is) being display. */
        s32            counters_1C[3];
        q19_12         field_28; // Multi-purpose? Used as alpha to fade between images in `Screen_BackgroundImgTransition`.
        q19_12         timer_2C; // Cutscene message timer?
        s32            cutsceneBorderState;
        s8             unused_34[4]; /** @unused */
        s_PlayerCombat playerCombat; // Information related to weapons and attack.
        s_PlayerWork   playerWork;
        s_SubCharacter npcs[NPC_COUNT_MAX];
        GsCOORDINATE2  playerBoneCoords[HarryBone_Count];
        GsCOORDINATE2  unkCoords_E30[5];                  // Might be part of previous array for 5 extra coords which go unused.
        GsCOORDINATE2  npcBoneCoords[NPC_BONE_COUNT_MAX]; // Dynamic coord buffer? 10 coords per NPC (given max of 6 NPCs).
        s8             npcFlagsId;                        // 1-based NPC ID for `npcFlags`.
        s8             loadingScreenIdx;
        s8             field_2282;                         /** `e_EventDataUnkState` */
        s8             sfxPairIdx_2283;                    /** `e_SfxPairIdx` | Index into `SFX_PAIRS`. */
        u16            charaGroupFlags[CHARA_GROUP_COUNT]; /** `e_CharaGroupFlags` */
                                                           // Enabling a flag for Larval Stalkers causes them to die.
        s32            field_228C[1];
        s32            npcFlags;       // Flags related to NPCs. Each bit corresponds to an `npcs` array entry.
        s8             unused_2294[4]; /** @unused */
        e_ProcessFlags processFlags;
        s32            field_229C; /** Dead code. Set to -1 when the player has been initalized and set to 0 when the player changes areas. Beyond that, this variable is unused. */
        int            bgmStatusFlags;
        int            flags_22A4; // `e_UnkSysFlags` | `UnkSysFlag_6` passed as "use through door cam" flag in `vcSetFirstCamWork`.
        GsCOORDINATE2  coord_22A8; // For particles only?
        GsCOORDINATE2  coord_22F8; // Likely related to above.
        s8             field_2348 : 8;
        s8             field_2349 : 8; // Particle spawn multiplier?
        bool           field_234A;
        u8             field_234B_0 : 4;
        u8             field_234B_4 : 4;
        s32            mapMsgTimer;
        bool           enableHighResGlyphs;
        bool           silentYesSelection;
        u32            inventoryItemSelectedIdx : 8;
        u32            flags_2352               : 8;
        s8             targetNpcIdx; /** Index of the NPC being targeted by the player. */
        s8             npcIdxs[CHARA_GROUP_COUNT];
        bool           enablePlayerMatchAnim; /** Activates the animation performed by Harry when lighting a match at the beginning of the game. */
        u8             playerStopFlags;       /** `e_PlayerStopFlags` */
        GsCOORDINATE2* field_235C;            // Bone related to pocket light.
        VECTOR3        pointLightPosition;    //                               } Often gets set from DMS cutscene data.
        GsCOORDINATE2* field_236C;            // Bone related to pocket light. }
        SVECTOR        pointLightRotation;    //                               }
        s16            pointLightIntensity;
        q3_12          cameraAngleY;
        q3_12          cameraAngleZ;
        s16            field_237E;
        q19_12         cameraRadiusXz;
        q19_12         cameraY;
        s_SysWork_2388 field_2388;
        s32            field_2510;
        //s_SysWork_2514  field_2514;
        u8             unused_254C[508]; // @unused Debug data?
        q3_12          bgmLayerVolumes[BGM_LAYER_COUNT];
        q19_12         field_275C;
        s32            field_2760;
        s32            field_2764;
    };

    extern s_SysWork               g_SysWork;
    extern s_GameWork              g_GameWork;
    extern s_GameWork* const       g_GameWorkConst;
    extern s_GameWork* const       g_GameWorkPtr;
    extern s_Savegame* const       g_SavegamePtr;
    extern s_ControllerData* const g_Controller0;
    extern s_ControllerData* const g_Controller1;

    extern u32 g_MapEventParam;

    /** @brief Sets `sysState` in `g_SysWork` for the next tick.
     *
     * @param sysState System state to set.
     * @return New system state.
     */
    static inline s32 SysWork_StateSetNext(e_SysState sysState)
    {
        s32 state;

        state                       =
        g_SysWork.sysState        = sysState;
        g_SysWork.counters_1C[2]          = 0;
        g_SysWork.sysStateSteps[0] = 0;
        g_SysWork.field_28          = 0;//Q12(0.0f);
        g_SysWork.sysStateSteps[1] = 0;
        g_SysWork.timer_2C          = 0;//Q12(0.0f);
        g_SysWork.sysStateSteps[2] = 0;
        return state;
    }

    /** @brief Increments `sysStateStep` in `g_SysWork` for the next tick.
     *
     * @param stepIdx The `sysStateStep` index to change.
     */
    static inline void SysWork_StateStepIncrement(s32 stepIdx)
    {
        if (stepIdx == 0)
        {
            g_SysWork.field_28          = 0;//Q12(0.0f);
            g_SysWork.sysStateSteps[1] = 0;
            g_SysWork.timer_2C          = 0;//Q12(0.0f);
            g_SysWork.sysStateSteps[2] = 0;
            g_SysWork.sysStateSteps[0]++;
        }
        else if (stepIdx == 1)
        {
            g_SysWork.timer_2C          = 0;//Q12(0.0f);
            g_SysWork.sysStateSteps[2] = 0;
            g_SysWork.sysStateSteps[1]++;
        }
        else
        {
            g_SysWork.sysStateSteps[2]++;
        }
    }

    /** @brief Sets `sysStateStep[stepIdx]` in `g_SysWork` for the next tick.
     *
     * @param stepIdx The `sysStateStep` index to change.
     * @param sysStateStep System state step to set.
     * @return New system state step.
     */
    static inline s32 SysWork_StateStepSet(s32 stepIdx, s32 sysStateStep)
    {
        s32 step;

        if (stepIdx == 0)
        {
            step                        =
            g_SysWork.sysStateSteps[0] = sysStateStep;
            g_SysWork.field_28          = 0;//Q12(0.0f);
            g_SysWork.sysStateSteps[1] = 0;
            g_SysWork.timer_2C          = 0;//Q12(0.0f);
            g_SysWork.sysStateSteps[2] = 0;
        }
        else if (stepIdx == 1)
        {
            step                        =
            g_SysWork.sysStateSteps[1] = sysStateStep;
            g_SysWork.timer_2C          = 0;//Q12(0.0f);
            g_SysWork.sysStateSteps[2] = 0;
        }
        else
        {
            step                        =
            g_SysWork.sysStateSteps[2] = sysStateStep;
        }

        return step;
    }

    /** @brief Resets `sysStateStep` in `g_SysWork` for the next tick. */
    static inline void SysWork_StateStepReset()
    {
        g_SysWork.sysStateSteps[0] = NO_VALUE;
        g_SysWork.field_28          = 0;//Q12(0.0f);
        g_SysWork.sysStateSteps[1] = 0;
        g_SysWork.timer_2C          = 0;//Q12(0.0f);
        g_SysWork.sysStateSteps[2] = 0;
    }

    /** @brief Sets an NPC flag in the `g_SysWork.npcFlags` bitfield.
     *
     * @param flagIdx Index of the NPC flag to set.
     */
    static inline void SysWork_NpcFlagSet(s32 flagIdx)
    {
        g_SysWork.npcFlags |= 1 << flagIdx;
    }

    /** @brief Clears an NPC flag in the `g_SysWork.npcFlags` bitfield.
     *
     * @param flagIdx Index of the NPC flag to clear.
     */
    static inline void SysWork_NpcFlagClear(s32 flagIdx)
    {
        CLEAR_FLAG(&g_SysWork.npcFlags, flagIdx);
    }

    /** @brief Clears state steps twice for some reason? Only used once below, others use regular `Game_StateSetNext`. */
    static inline void Game_StateSetNext_ClearStateSteps(e_GameState gameState)
    {
        e_GameState prevState;

        prevState = g_GameWork.gameState;

        g_GameWork.gameState        = gameState;
        g_SysWork.counters_1C[0]              = 0;
        g_SysWork.counters_1C[1]              = 0;
        g_GameWork.gameStateSteps[1] = 0;
        g_GameWork.gameStateSteps[2] = 0;

        SysWork_StateSetNext(SysState_Gameplay);

        g_GameWork.gameStateSteps[1] = 0;
        g_GameWork.gameStateSteps[2] = 0;
        g_GameWork.gameStateSteps[0] = prevState;
        g_GameWork.gameStatePrev    = prevState;
        g_GameWork.gameStateSteps[0] = 0;
    }

    /** @brief Sets the GameState to be used in the next game update.
     * Inlined into `stream` and `b_konami`.
     */
    static inline void Game_StateSetNext(e_GameState gameState)
    {
        e_GameState prevState;

        prevState = g_GameWork.gameState;

        g_GameWork.gameState        = gameState;
        g_SysWork.counters_1C[0]              = 0;
        g_SysWork.counters_1C[1]              = 0;
        g_GameWork.gameStateSteps[1] = 0;
        g_GameWork.gameStateSteps[2] = 0;

        SysWork_StateSetNext(SysState_Gameplay);

        g_GameWork.gameStateSteps[0] = prevState;
        g_GameWork.gameStatePrev    = prevState;
        g_GameWork.gameStateSteps[0] = 0;
    }

    /** @brief Returns the GameState to the previously used state.
     * Inlined into credits.
     */
    static inline void Game_StateSetPrevious()
    {
        e_GameState prevState;

        prevState = g_GameWork.gameState;

        g_SysWork.counters_1C[0]              = 0;
        g_SysWork.counters_1C[1]              = 0;
        g_GameWork.gameStateSteps[1] = 0;
        g_GameWork.gameStateSteps[2] = 0;

        SysWork_StateSetNext(SysState_Gameplay);

        g_GameWork.gameStateSteps[0] = prevState;
        g_GameWork.gameState        = g_GameWork.gameStatePrev;
        g_GameWork.gameStatePrev    = prevState;
        g_GameWork.gameStateSteps[0] = 0;
    }

    /** @brief Gets an event flag state from the savegame event flags array.
     *
     * @param flagIdx Event flag index.
     * @return Event flag state (`bool`).
     */
    #define Savegame_EventFlagGet(flagIdx) \
        (g_SavegamePtr->eventFlags[(flagIdx) >> 5] & (1 << ((flagIdx) & 0x1F)))

    /** @brief Gets an event flag state from the savegame event flags array.
     *
     * @note This alternate version shifts the flags array value by the flag index for some reason
     * and is required for some matches.
     *
     * @param flagIdx Event flag index.
     * @return Event flag state (`bool`).
     */
    #define Savegame_EventFlagGetAlt(flagIdx) \
        ((g_SavegamePtr->eventFlags[(flagIdx) >> 5] >> ((flagIdx) & 0x1F)) & (1 << 0))

    /** @brief Clears an event flag state in the savegame event flags array.
     *
     * @param flagIdx Event flag index.
     */
    #define Savegame_EventFlagClear(flagIdx) \
        (g_SavegamePtr->eventFlags[(flagIdx) >> 5] &= ~(1 << ((flagIdx) & 0x1F)))

    /** @brief Sets an event flag state in the savegame event flags array.
     *
     * @param flagIdx Event flag index.
     */
    #define Savegame_EventFlagSet(flagIdx) \
        (g_SavegamePtr->eventFlags[(flagIdx) >> 5] |= 1 << ((flagIdx) & 0x1F))

    /** @brief Gets a map marking state from the savegame map marking array.
     *
     * @param flagIdx Map marking index.
     * @return Map marking state (`bool`).
     */
    #define Savegame_MapMarkingGet(flagIdx) \
        (g_SavegamePtr->mapMarkingFlags[(flagIdx) >> 5] & (1 << ((flagIdx) & 0x1F)))

    /** @brief Clears a map marking state in the savegame map marking array.
     *
     * @param flagIdx Map marking index.
     */
    #define Savegame_MapMarkingClear(flagIdx) \
        (g_SavegamePtr->mapMarkingFlags[(flagIdx) >> 5] &= ~(1 << ((flagIdx) & 0x1F)))

    /** @brief Sets a map marking in the savegame map marking array.
     *
     * @param flagIdx Map marking index.
     */
    #define Savegame_MapMarkingSet(flagIdx) \
        (g_SavegamePtr->mapMarkingFlags[(flagIdx) >> 5] |= 1 << ((flagIdx) & 0x1F))

    /** @brief Sets an event flag state in the savegame event flags array.
     *
     * @note Some map event code only seems to work with this inline version.
     *
     * @param flagIdx Event flag index.
     */
    static inline void Savegame_EventFlagSetAlt(u32 flagIdx)
    {
        s16 localIdx;
        s16 localBit;

        localIdx = flagIdx / 32;
        localBit = flagIdx % 32;

        g_SavegamePtr->eventFlags[localIdx] |= 1 << localBit;
    }

    /** @brief Checks a flag state is `true` in the array of 16-bit flags.
     *
     * @param flags Flag array.
     * @param flagIdx Flag index.
     */
    static inline s32 Flags16b_IsSet(const u16* flags, s32 flagIdx)
    {
        // @bug `>> 5` divides `flagId` by 32 to get array index, but array contains 16-bit values. Maybe copy-paste from `u32` version of func.
        return (flags[flagIdx >> 5] >> (flagIdx & 0x1F)) & (1 << 0);
    }

    /** @brief Sets the animation of a character.
     *
     * @param chara Character to set animation for. TODO: Maybe should take `s_ModelAnim` instead? If fits better, also rename to `Anim_Set`.
     * @param animStatus Packed anim status. See `s_ModelAnim::status`.
     * @param keyframeIdx Active keyframe index.
     */
    static inline void Character_AnimSet(s_SubCharacter* chara, s32 animStatus, s32 keyframeIdx)
    {
        // TODO: Problem with header includes prevents `Q12` macro use.
        chara->model.anim.status      = animStatus;
        chara->model.anim.time        = keyframeIdx << 12;//Q12(keyframeIdx);
        chara->model.anim.keyframeIdx = keyframeIdx;
    }

    /** @brief Checks if the `s_SubCharacter*` has the given `flags` value set. */
    #define Chara_HasFlag(chara, flag) \
        ((chara)->flags & (flag))

    /** @brief Sets given animation flags for a model.
     *
     * @param model `s_Model` to update.
     * @param flag Flags to set.
     */
    #define Model_AnimFlagsSet(model, flags) \
        (model)->anim.flags |= (flags)

    /** @brief Clears given animation flags for a model.
     *
     * @param model `s_Model` to update.
     * @param flag Flags to clear.
     */
    #define Model_AnimFlagsClear(model, flags) \
        (model)->anim.flags &= ~(flags)

    /** @brief Updates a model anim if `model->stateStep` is 0.
     *
     * @param model Model to update.
     * @param animIdx Anim index to set.
     * @param isActive Active status to set.
     */
    static inline void Model_AnimStatusSet(s_Model* model, s32 animIdx, bool isActive)
    {
        if (model->stateStep == 0)
        {
            model->anim.status = ANIM_STATUS(animIdx, isActive);
            model->stateStep++;
        }
    }

    /** @brief Increments the anim status of a model anim.
     *
     * @param anim Anim to update.
     */
    static inline void ModelAnim_StatusIncrement(s_ModelAnim* anim)
    {
        anim->status++;
    }

    /** @brief Decrements the anim status of a model anim.
     *
     * @param anim Anim to update.
     */
    static inline void ModelAnim_StatusDecrement(s_ModelAnim* anim)
    {
        anim->status--;
    }

    /** @brief Similar to `Model_AnimStatusSet`, but also sets `anim.time` and `anim.keyframeIdx`
     * from the `animInfos` `s_AnimInfo` array.
     *
     * @param model Model to update.
     * @param animIdx Anim index to set.
     * @param isActive Active status to set.
     * @param animInfos Reference anim infos.
     * @param animInfosOffset Anim infos offset.
     */
    #define Model_AnimStatusKeyframeSet(model, animIdx, isActive, animInfos, animInfosOffset)                                 \
        if ((model).stateStep == 0)                                                                                           \
        {                                                                                                                     \
            (model).anim.status = ANIM_STATUS(animIdx, isActive);                                                             \
            (model).stateStep++;                                                                                              \
            (model).anim.time        = Q12((animInfos)[ANIM_STATUS(animIdx, isActive) + (animInfosOffset)].startKeyframeIdx); \
            (model).anim.keyframeIdx = (animInfos)[ANIM_STATUS(animIdx, (isActive) + (animInfosOffset))].startKeyframeIdx;    \
        }

    /** @brief Attempts to reset a humanoid NPC's anim state index to 0.
     *
     * @param chara Character to update.
     */
    static inline void Character_AnimStateReset(s_SubCharacter* chara)
    {
        // TODO: This uses `dahlia` part of union, but is most likely either a `human` part shared with all humanoid characters
        // or humanoids only share a small portion early in the union.
        if (chara->properties.dahlia.resetControlState)
        {
            chara->properties.dahlia.controlState      = 0;
            chara->model.stateStep                     = 0;
            chara->properties.dahlia.resetControlState = 0;
        }
    }

    /** @brief Clears a character's properties. TODO: Declare `i` here.
     *
     * @param chara Character to update.
     */
    #define Chara_PropsClear(chara)                             \
        for (i = 0; i < 16; i++)                                \
        {                                                       \
            chara->properties.dummy.properties_E8[i].val32 = 0; \
        }

    /** @brief Clears a character's damage field.
     *
     * @param chara Character to update.
     */
    #define Chara_DamageClear(chara)             \
        (chara)->damage.amount      = Q12(0.0f); \
        (chara)->damage.position.vz = Q12(0.0f); \
        (chara)->damage.position.vy = Q12(0.0f); \
        (chara)->damage.position.vx = Q12(0.0f)

    /** @brief Sets a character's received attack type.
     *
     * TODO: Could make this use `WEAPON_ATTACK` macro and take the values needed for that instead.
     * Maybe devs did similar thing, which is why this separate macro/inline is needed for a match?
     *
     * @param chara Character to update.
     * @param attack Attack type to set.
     */
    #define Chara_AttackReceivedSet(chara, attack) \
        (chara)->attackReceived = (attack)

    /** @brief Gets a character's received attack type.
     *
     * @param chara Character to update.
     */
    #define Chara_AttackReceivedGet(chara) \
        (chara)->attackReceived
}
