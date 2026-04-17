#include "Framework.h"
#include "Psx.h"
#include "Game/Bodyprog/Events/MapMsgDisplay.h"

#include "Game/Bodyprog/Bodyprog.h"

#include "Game/Bodyprog/Screen/ScreenData.h"
#include "Game/Bodyprog/Screen/ScreenDraw.h"
#include "Game/Bodyprog/Sound/SoundSystem.h"
#include "Game/Bodyprog/Text/TextDraw.h"
#include "Game/Main/FsQueue.h"

namespace Silent::Game
{
    static s32 g_MapMsg_CurrentIdx       = 0;
    static s16 g_MapMsg_SelectFlashTimer = 0;

    s_MapMsgSelect g_MapMsg_Select;
    u8             g_MapMsg_AudioLoadBlock;
    s8             g_MapMsg_SelectCancelIdx;

    s32 Gfx_MapMsg_Draw(s32 mapMsgIdx) // 0x800365B8
    {
        #define MSG_TIMER_MAX (Q12(524288.0f) - 1)
        constexpr int    FINISH_CUTSCENE = 0xFF;
        constexpr int    FINISH_MAP_MSG  = 0xFF;

        s32        temp_s1;
        bool       hasInput;
        s32        temp;
        s32        var_a1;
        static s32 stateMachineIdx0;
        static s32 stateMachineIdx1;
        static s32 msgDisplayLength;
        static s32 msgIdx;
        static s32 msgDisplayInc;
        static s32 D_800BCD74;

        // Check for user input.
        hasInput = false;
        if ((g_Controller0->btnsClicked_10 & (g_GameWorkPtr->config.controllerConfig_0.enter_0 |
                                            g_GameWorkPtr->config.controllerConfig_0.cancel_2)) ||
            (g_Controller0->btnsHeld_C & g_GameWorkPtr->config.controllerConfig_0.skip_4))
        {
            hasInput = true;
        }

        g_SysWork.playerWork.player.properties.player.gasWeaponPowerTimer_114 = Q12(0.0f);
        //func_8004C564(g_SysWork.playerCombat.weaponAttack, WEAPON_ATTACK(EquippedWeaponId_RockDrill, AttackInputType_Tap));

        if (msgIdx != mapMsgIdx)
        {
            g_SysWork.isMgsStringSet = false;
        }

        switch (g_SysWork.isMgsStringSet)
        {
            case false:
                g_SysWork.mapMsgTimer         = NO_VALUE;
                g_MapMsg_Select.maxIdx           = NO_VALUE;
                g_MapMsg_Select.selectedEntryIdx = 0;
                g_MapMsg_AudioLoadBlock            = 0;
                g_MapMsg_CurrentIdx                = mapMsgIdx;
                stateMachineIdx0          = 0;
                stateMachineIdx1          = 0;
                msgIdx                   = mapMsgIdx;
                msgDisplayLength             = 0;
                msgDisplayInc                = 2; // Advance 2 glyphs at a time.

                //Gfx_MapMsg_DefaultStringInfoSet();
                //var_a1 = Gfx_MapMsg_CalculateWidths(g_MapMsg_CurrentIdx);

                D_800BCD74 = 1;
                g_SysWork.isMgsStringSet = true;
                return MapMsgState_Finish;

            case true:
                if (g_SysWork.bgmStatusFlags & BgmStatusFlag_VoiceDialog)
                {
                    /*if (Sd_AudioStreamingCheck() == 4)
                    {
                        D_800BCD74 = 0;
                        break;
                    }*/

                    if (D_800BCD74 != 0)
                    {
                        break;
                    }
                }
                else
                {
                    D_800BCD74 = 0;
                }

                Gfx_StringSetColor(StringColorId_White);
                Gfx_StringSetPosition(40, 160);

                msgDisplayLength += msgDisplayInc;
                msgDisplayLength  = CLAMP(msgDisplayLength, 0, MAP_MESSAGE_DISPLAY_ALL_LENGTH);

                if (g_MapMsg_AudioLoadBlock != 0 && g_SysWork.mapMsgTimer > 0)
                {
                    g_SysWork.mapMsgTimer -= g_DeltaTimeRaw;
                    g_SysWork.mapMsgTimer  = CLAMP(g_SysWork.mapMsgTimer, Q12(0.0f), MSG_TIMER_MAX);
                }

                temp_s1 = stateMachineIdx0;
                if (temp_s1 == NO_VALUE)
                {
                    if (g_MapMsg_AudioLoadBlock == 0)
                    {
                        //Game_TimerUpdate();
                    }

                    temp = stateMachineIdx1;
                    if (temp == temp_s1)
                    {
                        if (g_MapMsg_Select.maxIdx == temp)
                        {
                            if (!((g_MapMsg_AudioLoadBlock & (1 << 0)) || !hasInput) ||
                                (g_MapMsg_AudioLoadBlock != 0 && g_SysWork.mapMsgTimer == 0))
                            {
                                stateMachineIdx1 = FINISH_MAP_MSG;

                                if (g_SysWork.bgmStatusFlags & BgmStatusFlag_VoiceDialog)
                                {
                                    SD_Call(19);
                                }
                                break;
                            }
                        }
                        else if (g_Controller0->btnsClicked_10 & g_GameWorkPtr->config.controllerConfig_0.cancel_2)
                        {
                            g_MapMsg_Select.maxIdx           = temp;
                            g_MapMsg_Select.selectedEntryIdx = g_MapMsg_SelectCancelIdx;

                            //Sd_PlaySfx(Sfx_MenuCancel, 0, Q8_CLAMPED(0.25f));

                            if (g_SysWork.silentYesSelection)
                            {
                                g_SysWork.silentYesSelection = false;
                            }

                            stateMachineIdx1 = FINISH_MAP_MSG;
                            break;
                        }
                        else if (g_Controller0->btnsClicked_10 & g_GameWorkPtr->config.controllerConfig_0.enter_0)
                        {
                            g_MapMsg_Select.maxIdx = temp;

                            if (g_MapMsg_Select.selectedEntryIdx == (s8)g_MapMsg_SelectCancelIdx)
                            {
                                //Sd_PlaySfx(Sfx_MenuCancel, 0, Q8_CLAMPED(0.25f));
                            }
                            else if (!g_SysWork.silentYesSelection)
                            {
                                //Sd_PlaySfx(Sfx_MenuConfirm, 0, Q8_CLAMPED(0.25f));
                            }

                            if (g_SysWork.silentYesSelection)
                            {
                                g_SysWork.silentYesSelection = false;
                            }

                            stateMachineIdx1 = FINISH_MAP_MSG;
                            break;
                        }
                    }
                    else if ((!(g_MapMsg_AudioLoadBlock & (1 << 0)) && hasInput && g_MapMsg_Select.maxIdx != 0) ||
                            (g_MapMsg_AudioLoadBlock != 0 && g_SysWork.mapMsgTimer == 0))
                    {
                        if (g_MapMsg_Select.maxIdx != NO_VALUE)
                        {
                            g_MapMsg_Select.maxIdx  = NO_VALUE;
                            stateMachineIdx1 = FINISH_MAP_MSG;
                            break;
                        }

                        g_MapMsg_CurrentIdx++;
                        g_SysWork.mapMsgTimer = g_MapMsg_Select.maxIdx;

                        //var_a1 = Gfx_MapMsg_CalculateWidths(g_MapMsg_CurrentIdx);

                        msgDisplayLength = 0;
                        stateMachineIdx0 = 0;

                        if (g_MapMsg_AudioLoadBlock == MapMsgAudioLoadBlock_J2)
                        {
                            D_800BCD74 = 0;
                            return MapMsgState_Idle;
                        }

                        if (g_SysWork.bgmStatusFlags & BgmStatusFlag_VoiceDialog)
                        {
                            SD_Call(19);
                        }

                        D_800BCD74 = 1;
                        return MapMsgState_Finish;
                    }
                }
                else
                {
                    if (hasInput)
                    {
                        msgDisplayLength = MAP_MESSAGE_DISPLAY_ALL_LENGTH;
                    }
                }

                stateMachineIdx0 = 0;
                stateMachineIdx1 = Gfx_MapMsg_SelectionUpdate(g_MapMsg_CurrentIdx, &msgDisplayLength);

                if (stateMachineIdx1 != 0 && stateMachineIdx1 < MapMsgCode_Select4)
                {
                    stateMachineIdx0 = NO_VALUE;
                }
        }

        if (stateMachineIdx1 != FINISH_MAP_MSG)
        {
            return MapMsgState_Idle;
        }

        g_SysWork.isMgsStringSet            = false;
        g_SysWork.enableHighResGlyphs = false;
        msgDisplayLength               = 0;

        if (g_SysWork.bgmStatusFlags & BgmStatusFlag_VoiceDialog)
        {
            D_800BCD74 = 1;
        }

        return g_MapMsg_Select.selectedEntryIdx + 1;
    }

    s32 Gfx_MapMsg_SelectionUpdate(u8 mapMsgIdx, s32* arg1) // 0x80036B5C
    {
        constexpr int STRING_LINE_OFFSET = 16;

        s32 i;
        s32 mapMsgCode;

        mapMsgCode = //Gfx_MapMsg_StringDraw(g_MapOverlayHeader.mapMessages_30[mapMsgIdx], *arg1);

        g_MapMsg_SelectFlashTimer += g_DeltaTimeRaw;
        if (g_MapMsg_SelectFlashTimer >= Q12(0.5f))
        {
            g_MapMsg_SelectFlashTimer -= Q12(0.5f);
        }

        switch (mapMsgCode)
        {
            case NO_VALUE:
            case MapMsgCode_None:
                g_MapMsg_SelectFlashTimer = Q12(0.0f);
                break;

            case MapMsgCode_Select2:
            case MapMsgCode_Select3:
            case MapMsgCode_Select4:
                g_MapMsg_Select.maxIdx  = 1;
                g_MapMsg_SelectCancelIdx = (mapMsgCode == 3) ? 2 : 1;

                if (mapMsgCode == MapMsgCode_Select4)
                {
                    // Shows selection prompt with map messages at indices 0 and 1.
                    // All maps have "Yes" and "No" as messages 0 and 1, respectively.
                    for (i = 0; i < 2; i++)
                    {
                        if (g_MapMsg_Select.selectedEntryIdx == i)
                        {
                            Gfx_StringSetColor((e_StringColorId)(((g_MapMsg_SelectFlashTimer >> 10) * 3) + 4));
                        }
                        else
                        {
                            Gfx_StringSetColor(StringColorId_White);
                        }

                        Gfx_StringSetPosition(32, (STRING_LINE_OFFSET * i) + 98);
                        Gfx_StringDraw(g_MapOverlayHeader.mapMessages_30[i], MAP_MESSAGE_DISPLAY_ALL_LENGTH);
                    }

                    mapMsgCode = 2;
                }
                else
                {
                    // Shows selection prompt with 2 or 3 map messages from current index + 1/2/3.
                    // Requires prompt options to be arranged sequentially in the map message array, e.g.
                    // `[idx]`:     "Select one of 3 options. ~S3"
                    // `[idx + 1]`: "Option 1"
                    // `[idx + 2]`: "Option 2"
                    // `[idx + 3]`: "Option 3"
                    for (i = 0; i < mapMsgCode; i++)
                    {
                        if (g_MapMsg_Select.selectedEntryIdx == i)
                        {
                            Gfx_StringSetColor((e_StringColorId)(((g_MapMsg_SelectFlashTimer >> 10) * 3) + 4));
                        }
                        else
                        {
                            Gfx_StringSetColor(StringColorId_White);
                        }

                        Gfx_StringSetPosition(32, (STRING_LINE_OFFSET * i) + 96);
                        Gfx_StringDraw(g_MapOverlayHeader.mapMessages_30[(mapMsgIdx + i) + 1], MAP_MESSAGE_DISPLAY_ALL_LENGTH);
                    }
                }

                if (g_Controller0->btnsClicked_10 & ControllerFlag_LStickUp &&
                    g_MapMsg_Select.selectedEntryIdx != 0)
                {
                    g_MapMsg_SelectFlashTimer = Q12(0.0f);
                    g_MapMsg_Select.selectedEntryIdx--;

                    //Sd_PlaySfx(Sfx_MenuMove, 0, Q8_CLAMPED(0.25f));
                }

                if (g_Controller0->btnsClicked_10 & ControllerFlag_LStickDown &&
                    g_MapMsg_Select.selectedEntryIdx != (mapMsgCode - 1))
                {
                    g_MapMsg_SelectFlashTimer = Q12(0.0f);
                    g_MapMsg_Select.selectedEntryIdx++;

                    //Sd_PlaySfx(Sfx_MenuMove, 0, Q8_CLAMPED(0.25f));
                }

                mapMsgCode = NO_VALUE;
                break;

            case MapMsgCode_DisplayAll:
                *arg1 = MAP_MESSAGE_DISPLAY_ALL_LENGTH;
                break;
        }

        return mapMsgCode;
    }

    void func_8003708C(s16* ptr0, u16* ptr1) // 0x8003708C
    {
        s32 var0;
        s16 var1;
        s32 var3;
        s32 shift;
        s32 i;

        var0 = 0;
        var1 = 0;

        for (i = 0; i < 12; i++)
        {
            shift = (i & 0x3) * 4;
            var3  = (*ptr1 >> shift) & 0xF;
            if (i != 0 && var3 == 11 && var0 != 0)
            {
                var1 |= 11 << shift;
            }

            var0 = 0;
            if (var3 != 0 && var3 != 11)
            {
                var1 |= 11 << shift;
                var0  = 1;
            }

            if ((i & 0x3) == 3 || i == 12)
            {
                ptr1++;
                *ptr0++ = var1;
                var1    = 0;
            }
        }
    }

    void func_80037124() // 0x80037124
    {
        g_MapMsg_Select.maxIdx = NO_VALUE;
        //func_8003652C();
        //DrawSync(SyncMode_Wait);
    }
}
