#pragma once

namespace Silent::Game
{
    /** @brief Handles the general controller
     * handling, controller IO stream and state.
     * Game controller logic seems to have been split among many parts of
     * the game depending on the state or screen the player is using.
     * For example, the controller logic for the inventory screen is handled separately from the
     * one for the in-game state.
     */

    constexpr int  INPUT_ACTION_COUNT   = 14;
    constexpr int  CONTROLLER_COUNT_MAX = 2;
    constexpr q0_7 STICK_THRESHOLD      = FP_STICK(0.5f);

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
        s_AnalogController analogController;
        s32                pulseTicks;
        s32                heldBtnFlags;      /** `e_ControllerFlags` */
        s32                clickedBtnFlags;   /** `e_ControllerFlags` */
        s32                releasedBtnFlags;  /** `e_ControllerFlags` */
        s32                pulsedBtnFlags;    /** `e_ControllerFlags` */
        s32                pulsedGuiBtnFlags; /** `e_ControllerFlags` */
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
        u16 cancel;
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

    extern s_ControllerData* const g_Controller0;
    extern s_ControllerData* const g_Controller1;

    void Joy_Init();

    void Joy_ReadP1();

    void Joy_Update();

    void Joy_ControllerDataUpdate();

    void ControllerData_AnalogToDigital(s_ControllerData* cont, bool arg1);
}
