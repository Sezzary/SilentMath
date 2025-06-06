#pragma once

namespace Silent::Input
{
    enum class EventId
    {
        // Keyboard

        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,
        Row1,
        Row2,
        Row3,
        Row4,
        Row5,
        Row6,
        Row7,
        Row8,
        Row9,
        Row0,
        Return,
        Escape,
        Backspace,
        Tab,
        Space,
        Grave,
        Minus,
        Equals,
        BracketLeft,
        BracketRight,
        Backslash,
        Semicolon,
        Apostrophe,
        Comma,
        Period,
        Slash,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        PrintScreen,
        Home,
        PageUp,
        Delete,
        End,
        PageDown,
        Right,
        Left,
        Down,
        Up,
        PadDivide,
        PadMultiply,
        PadMinus,
        PadPlus,
        PadEnter,
        Pad1,
        Pad2,
        Pad3,
        Pad4,
        Pad5,
        Pad6,
        Pad7,
        Pad8,
        Pad9,
        Pad0,
        PadPeriod,
        Ctrl,
        Shift,
        Alt,

        // Mouse

        MouseClickLeft,
        MouseClickMiddle,
        MouseClickRight,
        MouseScrollUp,
        MouseScrollDown,
        MouseScrollLeft,
        MouseScrollRight,
        MouseUp,
        MouseDown,
        MouseLeft,
        MouseRight,

        // Gamepad

        GamepadSouth,
        GamepadEast,
        GamepadWest,
        GamepadNorth,
        GamepadSelect,
        GamepadStart,
        GamepadStickLeft,
        GamepadStickRight,
        GamepadShoulderLeft,
        GamepadShoulderRight,
        GamepadDpadUp,
        GamepadDpadDown,
        GamepadDpadLeft,
        GamepadDpadRight,
        GamepadPaddleRight0,
        GamepadPaddleLeft0,
        GamepadPaddleRight1,
        GamepadPaddelLeft1,
        GamepadMisc0,
        GamepadMisc1,
        GamepadMisc2,
        GamepadMisc3,
        GamepadMisc4,
        GamepadStickLeftLeft,
        GamepadStickLeftRight,
        GamepadStickLeftUp,
        GamepadStickLeftDown,
        GamepadStickRightLeft,
        GamepadStickRightRight,
        GamepadStickRightUp,
        GamepadStickRightDown,
        GamepadTriggerLeft,
        GamepadTriggerRight,

        Count
    };

    enum class EventGroupId
    {
        KeyboardMouse,
        Gamepad
    };

    extern const std::unordered_map<EventGroupId, std::vector<EventId>> EVENT_ID_GROUPS;

    extern const std::vector<EventId> KEYBOARD_MOUSE_EVENT_IDS;
    extern const std::vector<EventId> GAMEPAD_EVENT_IDS;

    extern const std::vector<SDL_Scancode>      VALID_KEYBOARD_SCAN_CODES;
    extern const std::vector<int>               VALID_KEYBOARD_MODIFIER_CODES;
    extern const std::vector<int>               VALID_MOUSE_BUTTON_CODES;
    extern const std::vector<SDL_GamepadButton> VALID_GAMEPAD_BUTTON_CODES;
    extern const std::vector<SDL_GamepadAxis>   VALID_GAMEPAD_STICK_AXIS_CODES;
    extern const std::vector<SDL_GamepadAxis>   VALID_GAMEPAD_TRIGGER_AXIS_CODES;

    const std::string& GetEventName(EventId eventId);
}
