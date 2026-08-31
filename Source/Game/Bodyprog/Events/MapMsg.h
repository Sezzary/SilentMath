#pragma once

#include "Game/Bodyprog/Text/TextDraw.h"

namespace Silent::Game
{
    constexpr int DEFAULT_MAP_MESSAGE_LENGTH     = 99;
    constexpr int MAP_MESSAGE_DISPLAY_ALL_LENGTH = 400; /** Long string length is used to display a whole message instantly without a rollout. */

    /** @brief Map message indices.
     *
     * @note Each map has its own messages, with the first 15 hardcoded to be the same.
     */
    enum e_MapMsgIdx
    {
        MapMsgIdx_Yes               = 0,
        MapMsgIdx_No                = 1,
        MapMsgIdx_SaveGame          = 2, /** "Someday, someone may experience..." */
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
        MapMsgIdx_NowMaking         = 14  /** @unused? */
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

    struct s_MapMsgSelect
    {
        s8 maxIdx;
        u8 selectedEntryIdx;
    };

    s32 Gfx_MapMsg_Draw(s32 mapMsgIdx);

    s32 Gfx_MapMsg_SelectionUpdate(u8 mapMsgIdx, s32* displayLength);

    void func_8003708C(s16* ptr0, u16* ptr1);

    void func_80037124();
}
