#include "Framework.h"
#include "Engine/Input/Event.h"

namespace Silent::Input
{
    enum class VendorType
    {
        Generic,
        Xbox,
        Nintendo,
        Sony
    };

    const std::unordered_map<EventGroupId, std::vector<EventId>> EVENT_ID_GROUPS =
    {
        {
            EventGroupId::KeyboardMouse,
            {
                EventId::A,
                EventId::B,
                EventId::C,
                EventId::D,
                EventId::E,
                EventId::F,
                EventId::G,
                EventId::H,
                EventId::I,
                EventId::J,
                EventId::K,
                EventId::L,
                EventId::M,
                EventId::N,
                EventId::O,
                EventId::P,
                EventId::Q,
                EventId::R,
                EventId::S,
                EventId::T,
                EventId::U,
                EventId::V,
                EventId::W,
                EventId::X,
                EventId::Y,
                EventId::Z,
                EventId::Row1,
                EventId::Row2,
                EventId::Row3,
                EventId::Row4,
                EventId::Row5,
                EventId::Row6,
                EventId::Row7,
                EventId::Row8,
                EventId::Row9,
                EventId::Row0,
                EventId::Return,
                EventId::Escape,
                EventId::Backspace,
                EventId::Tab,
                EventId::Space,
                EventId::Grave,
                EventId::Minus,
                EventId::Equals,
                EventId::BracketLeft,
                EventId::BracketRight,
                EventId::Backslash,
                EventId::Semicolon,
                EventId::Apostrophe,
                EventId::Comma,
                EventId::Period,
                EventId::Slash,
                EventId::F1,
                EventId::F2,
                EventId::F3,
                EventId::F4,
                EventId::F5,
                EventId::F6,
                EventId::F7,
                EventId::F8,
                EventId::F9,
                EventId::F10,
                EventId::F11,
                EventId::F12,
                EventId::PrintScreen,
                EventId::Home,
                EventId::PageUp,
                EventId::Delete,
                EventId::End,
                EventId::PageDown,
                EventId::Right,
                EventId::Left,
                EventId::Down,
                EventId::Up,
                EventId::PadDivide,
                EventId::PadMultiply,
                EventId::PadMinus,
                EventId::PadPlus,
                EventId::PadEnter,
                EventId::Pad1,
                EventId::Pad2,
                EventId::Pad3,
                EventId::Pad4,
                EventId::Pad5,
                EventId::Pad6,
                EventId::Pad7,
                EventId::Pad8,
                EventId::Pad9,
                EventId::Pad0,
                EventId::PadPeriod,
                EventId::Ctrl,
                EventId::Shift,
                EventId::Alt,

                EventId::MouseClickLeft,
                EventId::MouseClickMiddle,
                EventId::MouseClickRight,
                EventId::MouseScrollUp,
                EventId::MouseScrollDown,
                EventId::MouseScrollLeft,
                EventId::MouseScrollRight,
                EventId::MouseUp,
                EventId::MouseDown,
                EventId::MouseLeft,
                EventId::MouseRight
            }
        },
        {
            EventGroupId::Gamepad,
            {
                EventId::GamepadSouth,
                EventId::GamepadEast,
                EventId::GamepadWest,
                EventId::GamepadNorth,
                EventId::GamepadSelect,
                EventId::GamepadStart,
                EventId::GamepadStickLeft,
                EventId::GamepadStickRight,
                EventId::GamepadShoulderLeft,
                EventId::GamepadShoulderRight,
                EventId::GamepadDpadUp,
                EventId::GamepadDpadDown,
                EventId::GamepadDpadLeft,
                EventId::GamepadDpadRight,
                EventId::GamepadPaddleRight0,
                EventId::GamepadPaddleLeft0,
                EventId::GamepadPaddleRight1,
                EventId::GamepadPaddelLeft1,
                EventId::GamepadMisc0,
                EventId::GamepadMisc1,
                EventId::GamepadMisc2,
                EventId::GamepadMisc3,
                EventId::GamepadMisc4,
                EventId::GamepadStickLeftLeft,
                EventId::GamepadStickLeftRight,
                EventId::GamepadStickLeftUp,
                EventId::GamepadStickLeftDown,
                EventId::GamepadStickRightLeft,
                EventId::GamepadStickRightRight,
                EventId::GamepadStickRightUp,
                EventId::GamepadStickRightDown,
                EventId::GamepadTriggerLeft,
                EventId::GamepadTriggerRight
            }
        }
    };

    const std::vector<SDL_Scancode> VALID_KEYBOARD_SCAN_CODES = 
    {
        SDL_SCANCODE_A,
        SDL_SCANCODE_B,
        SDL_SCANCODE_C,
        SDL_SCANCODE_D,
        SDL_SCANCODE_E,
        SDL_SCANCODE_F,
        SDL_SCANCODE_G,
        SDL_SCANCODE_H,
        SDL_SCANCODE_I,
        SDL_SCANCODE_J,
        SDL_SCANCODE_K,
        SDL_SCANCODE_L,
        SDL_SCANCODE_M,
        SDL_SCANCODE_N,
        SDL_SCANCODE_O,
        SDL_SCANCODE_P,
        SDL_SCANCODE_Q,
        SDL_SCANCODE_R,
        SDL_SCANCODE_S,
        SDL_SCANCODE_T,
        SDL_SCANCODE_U,
        SDL_SCANCODE_V,
        SDL_SCANCODE_W,
        SDL_SCANCODE_X,
        SDL_SCANCODE_Y,
        SDL_SCANCODE_Z,
        SDL_SCANCODE_1,
        SDL_SCANCODE_2,
        SDL_SCANCODE_3,
        SDL_SCANCODE_4,
        SDL_SCANCODE_5,
        SDL_SCANCODE_6,
        SDL_SCANCODE_7,
        SDL_SCANCODE_8,
        SDL_SCANCODE_9,
        SDL_SCANCODE_0,
        SDL_SCANCODE_RETURN,
        SDL_SCANCODE_ESCAPE,
        SDL_SCANCODE_BACKSPACE,
        SDL_SCANCODE_TAB,
        SDL_SCANCODE_SPACE,
        SDL_SCANCODE_GRAVE,
        SDL_SCANCODE_MINUS,
        SDL_SCANCODE_EQUALS,
        SDL_SCANCODE_LEFTBRACKET,
        SDL_SCANCODE_RIGHTBRACKET,
        SDL_SCANCODE_BACKSLASH,
        SDL_SCANCODE_SEMICOLON,
        SDL_SCANCODE_APOSTROPHE,
        SDL_SCANCODE_COMMA,
        SDL_SCANCODE_PERIOD,
        SDL_SCANCODE_SLASH,
        SDL_SCANCODE_F1,
        SDL_SCANCODE_F2,
        SDL_SCANCODE_F3,
        SDL_SCANCODE_F4,
        SDL_SCANCODE_F5,
        SDL_SCANCODE_F6,
        SDL_SCANCODE_F7,
        SDL_SCANCODE_F8,
        SDL_SCANCODE_F9,
        SDL_SCANCODE_F10,
        SDL_SCANCODE_F11,
        SDL_SCANCODE_F12,
        SDL_SCANCODE_PRINTSCREEN,
        SDL_SCANCODE_HOME,
        SDL_SCANCODE_PAGEUP,
        SDL_SCANCODE_DELETE,
        SDL_SCANCODE_END,
        SDL_SCANCODE_PAGEDOWN,
        SDL_SCANCODE_RIGHT,
        SDL_SCANCODE_LEFT,
        SDL_SCANCODE_DOWN,
        SDL_SCANCODE_UP,
        SDL_SCANCODE_KP_DIVIDE,
        SDL_SCANCODE_KP_MULTIPLY,
        SDL_SCANCODE_KP_MINUS,
        SDL_SCANCODE_KP_PLUS,
        SDL_SCANCODE_KP_ENTER,
        SDL_SCANCODE_KP_1,
        SDL_SCANCODE_KP_2,
        SDL_SCANCODE_KP_3,
        SDL_SCANCODE_KP_4,
        SDL_SCANCODE_KP_5,
        SDL_SCANCODE_KP_6,
        SDL_SCANCODE_KP_7,
        SDL_SCANCODE_KP_8,
        SDL_SCANCODE_KP_9,
        SDL_SCANCODE_KP_0,
        SDL_SCANCODE_KP_PERIOD
    };

    const std::vector<int> VALID_KEYBOARD_MODIFIER_CODES =
    {
        SDL_KMOD_CTRL,
        SDL_KMOD_SHIFT,
        SDL_KMOD_ALT
    };

    const std::vector<int> VALID_MOUSE_BUTTON_CODES =
    {
        SDL_BUTTON_LEFT,
        SDL_BUTTON_MIDDLE,
        SDL_BUTTON_RIGHT
    };

    const std::vector<SDL_GamepadButton> VALID_GAMEPAD_BUTTON_CODES =
    {
        SDL_GAMEPAD_BUTTON_SOUTH,
        SDL_GAMEPAD_BUTTON_EAST,
        SDL_GAMEPAD_BUTTON_WEST,
        SDL_GAMEPAD_BUTTON_NORTH,
        SDL_GAMEPAD_BUTTON_BACK,
        SDL_GAMEPAD_BUTTON_START,
        SDL_GAMEPAD_BUTTON_LEFT_STICK,
        SDL_GAMEPAD_BUTTON_RIGHT_STICK,
        SDL_GAMEPAD_BUTTON_LEFT_SHOULDER,
        SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER,
        SDL_GAMEPAD_BUTTON_DPAD_UP,
        SDL_GAMEPAD_BUTTON_DPAD_DOWN,
        SDL_GAMEPAD_BUTTON_DPAD_LEFT,
        SDL_GAMEPAD_BUTTON_DPAD_RIGHT,
        SDL_GAMEPAD_BUTTON_RIGHT_PADDLE1,
        SDL_GAMEPAD_BUTTON_LEFT_PADDLE1,
        SDL_GAMEPAD_BUTTON_RIGHT_PADDLE2,
        SDL_GAMEPAD_BUTTON_LEFT_PADDLE2,
        SDL_GAMEPAD_BUTTON_MISC2,
        SDL_GAMEPAD_BUTTON_MISC3,
        SDL_GAMEPAD_BUTTON_MISC4,
        SDL_GAMEPAD_BUTTON_MISC5,
        SDL_GAMEPAD_BUTTON_MISC6
    };

    const std::vector<SDL_GamepadAxis> VALID_GAMEPAD_STICK_AXIS_CODES =
    {
        SDL_GAMEPAD_AXIS_LEFTX,
        SDL_GAMEPAD_AXIS_LEFTY,
        SDL_GAMEPAD_AXIS_RIGHTX,
        SDL_GAMEPAD_AXIS_RIGHTY
    };
    
    const std::vector<SDL_GamepadAxis> VALID_GAMEPAD_TRIGGER_AXIS_CODES =
    {
        SDL_GAMEPAD_AXIS_LEFT_TRIGGER,
        SDL_GAMEPAD_AXIS_RIGHT_TRIGGER
    };

    static const auto EVENT_NAMES = std::unordered_map<EventId, std::vector<std::string>>
    {   
        { EventId::A,                      { "A" } },
        { EventId::B,                      { "B" } },
        { EventId::C,                      { "C" } },
        { EventId::D,                      { "D" } },
        { EventId::E,                      { "E" } },
        { EventId::F,                      { "F" } },
        { EventId::G,                      { "G" } },
        { EventId::H,                      { "H" } },
        { EventId::I,                      { "I" } },
        { EventId::J,                      { "J" } },
        { EventId::K,                      { "K" } },
        { EventId::L,                      { "L" } },
        { EventId::M,                      { "M" } },
        { EventId::N,                      { "N" } },
        { EventId::O,                      { "O" } },
        { EventId::P,                      { "P" } },
        { EventId::Q,                      { "Q" } },
        { EventId::R,                      { "R" } },
        { EventId::S,                      { "S" } },
        { EventId::T,                      { "T" } },
        { EventId::U,                      { "U" } },
        { EventId::V,                      { "V" } },
        { EventId::W,                      { "W" } },
        { EventId::X,                      { "X" } },
        { EventId::Y,                      { "Y" } },
        { EventId::Z,                      { "Z" } },
        { EventId::Row1,                   { "1" } },
        { EventId::Row2,                   { "2" } },
        { EventId::Row3,                   { "3" } },
        { EventId::Row4,                   { "4" } },
        { EventId::Row5,                   { "5" } },
        { EventId::Row6,                   { "6" } },
        { EventId::Row7,                   { "7" } },
        { EventId::Row8,                   { "8" } },
        { EventId::Row9,                   { "9" } },
        { EventId::Row0,                   { "0" } },
        { EventId::Return,                 { "Return" } },
        { EventId::Escape,                 { "Escape" } },
        { EventId::Backspace,              { "Backspace" } },
        { EventId::Tab,                    { "Tab" } },
        { EventId::Space,                  { "Space" } },
        { EventId::Minus,                  { "Minus" } },
        { EventId::Equals,                 { "Equals" } },
        { EventId::BracketLeft,            { "Left Bracket" } },
        { EventId::BracketRight,           { "Right Bracket" } },
        { EventId::Backslash,              { "Backslash" } },
        { EventId::Semicolon,              { "Semicolon" } },
        { EventId::Apostrophe,             { "Apostrophe" } },
        { EventId::Comma,                  { "Comma" } },
        { EventId::Period,                 { "Period" } },
        { EventId::Slash,                  { "Slash" } },
        { EventId::F1,                     { "F1" } },
        { EventId::F2,                     { "F2" } },
        { EventId::F3,                     { "F3" } },
        { EventId::F4,                     { "F4" } },
        { EventId::F5,                     { "F5" } },
        { EventId::F6,                     { "F6" } },
        { EventId::F7,                     { "F7" } },
        { EventId::F8,                     { "F8" } },
        { EventId::F9,                     { "F9" } },
        { EventId::F10,                    { "F10" } },
        { EventId::F11,                    { "F11" } },
        { EventId::F12,                    { "F12" } },
        { EventId::PrintScreen,            { "PrintScreen" } },
        { EventId::Home,                   { "Home" } },
        { EventId::PageUp,                 { "Page Up" } },
        { EventId::Delete,                 { "Delete" } },
        { EventId::End,                    { "End" } },
        { EventId::PageDown,               { "Page Down" } },
        { EventId::Right,                  { "Right" } },
        { EventId::Left,                   { "Left" } },
        { EventId::Down,                   { "Down" } },
        { EventId::Up,                     { "Up" } },
        { EventId::PadDivide,              { "Numpad Divide" } },
        { EventId::PadMultiply,            { "Numpad Multiply" } },
        { EventId::PadMinus,               { "Numpad Minus" } },
        { EventId::PadPlus,                { "Numpad Plus" } },
        { EventId::PadEnter,               { "Numpad Enter" } },
        { EventId::Pad1,                   { "Numpad 1" } },
        { EventId::Pad2,                   { "Numpad 2" } },
        { EventId::Pad3,                   { "Numpad 3" } },
        { EventId::Pad4,                   { "Numpad 4" } },
        { EventId::Pad5,                   { "Numpad 5" } },
        { EventId::Pad6,                   { "Numpad 6" } },
        { EventId::Pad7,                   { "Numpad 7" } },
        { EventId::Pad8,                   { "Numpad 8" } },
        { EventId::Pad9,                   { "Numpad 9" } },
        { EventId::Pad0,                   { "Numpad 0" } },
        { EventId::PadPeriod,              { "Numpad Period" } },
        { EventId::Ctrl,                   { "Ctrl" } },
        { EventId::Shift,                  { "Shift" } },
        { EventId::Alt,                    { "Alt" } },

        { EventId::MouseClickLeft,         { "Left Click" } },
        { EventId::MouseClickMiddle,       { "Middle Click" } },
        { EventId::MouseClickRight,        { "Right Click" } },
        { EventId::MouseScrollUp,          { "Scroll Up" } },
        { EventId::MouseScrollDown,        { "Scroll Down" } },
        { EventId::MouseScrollLeft,        { "Scroll Left" } },
        { EventId::MouseScrollRight,       { "Scroll Right" } },
        { EventId::MouseUp,                { "Mouse Up" } },
        { EventId::MouseDown,              { "Mouse Down" } },
        { EventId::MouseLeft,              { "Mouse Left" } },
        { EventId::MouseRight,             { "Mouse Right" } },

        { EventId::GamepadSouth,           { "South", "Gamepad A", "Gamepad B", "Cross" } },
        { EventId::GamepadEast,            { "East", "Gamepad B", "Gamepad A", "Circle" } },
        { EventId::GamepadWest,            { "West", "Gamepad X", "Gamepad Y", "Square" } },
        { EventId::GamepadNorth,           { "North", "Gamepad Y", "Gamepad X", "Triangle" } },
        { EventId::GamepadSelect,          { "Select", "Back", "Select", "Select" } },
        { EventId::GamepadStart,           { "Start" } },
        { EventId::GamepadStickLeft,       { "Left Stick In" } },
        { EventId::GamepadStickRight,      { "Right Stick In" } },
        { EventId::GamepadShoulderLeft,    { "Left Shoulder", "LB", "L", "L1" } },
        { EventId::GamepadShoulderRight,   { "Right Shoulder", "RB", "R", "R1" } },
        { EventId::GamepadDpadUp,          { "D-Pad Up" } },
        { EventId::GamepadDpadDown,        { "D-Pad Down" } },
        { EventId::GamepadDpadLeft,        { "D-Pad Left" } },
        { EventId::GamepadDpadRight,       { "D-Pad Right" } },
        { EventId::GamepadPaddleRight0,    { "Right Primary Paddle" } },
        { EventId::GamepadPaddleLeft0,     { "Left Primary Paddle" } },
        { EventId::GamepadPaddleRight1,    { "Right Secondary Paddle" } },
        { EventId::GamepadPaddelLeft1,     { "Left Secondary Paddle" } },
        { EventId::GamepadMisc0,           { "Gamepad Misc 1" } },
        { EventId::GamepadMisc1,           { "Gamepad Misc 2" } },
        { EventId::GamepadMisc2,           { "Gamepad Misc 3" } },
        { EventId::GamepadMisc3,           { "Gamepad Misc 4" } },
        { EventId::GamepadMisc4,           { "Gamepad Misc 5" } },
        { EventId::GamepadStickLeftLeft,   { "Left Stick Left" } },
        { EventId::GamepadStickLeftRight,  { "Left Stick Right" } },
        { EventId::GamepadStickLeftUp,     { "Left Stick Up" } },
        { EventId::GamepadStickLeftDown,   { "Left Stick Down" } },
        { EventId::GamepadStickRightLeft,  { "Right Stick Left" } },
        { EventId::GamepadStickRightRight, { "Right Stick Right" } },
        { EventId::GamepadStickRightUp,    { "Right Stick Up" } },
        { EventId::GamepadStickRightDown,  { "Right Stick Down" } },
        { EventId::GamepadTriggerLeft,     { "Left Trigger", "LT", "ZL", "L2" } },
        { EventId::GamepadTriggerRight,    { "Right Trigger", "RT", "ZR", "R2" } }
    };

    static VendorType GetGamepadVendorType()
    {
        constexpr ushort XBOX_VENDOR_ID     = 0x045E;
        constexpr ushort NINTENDO_VENDOR_ID = 0x057E;
        constexpr ushort SONY_VENDOR_ID     = 0x054C;

        auto* gamepad = SDL_OpenGamepad(0);

        // Determine vendor type.
        auto type = VendorType::Generic;
        switch (SDL_GetGamepadVendor(gamepad))
        {
            case XBOX_VENDOR_ID:
            {
                type = VendorType::Xbox;
                break;
            }

            case NINTENDO_VENDOR_ID:
            {
                type = VendorType::Nintendo;
                break;
            }

            case SONY_VENDOR_ID:
            {
                type = VendorType::Sony;
                break;
            }

            default:
            {
                type = VendorType::Generic;
                break;
            }
        }

        SDL_CloseGamepad(gamepad);
        return type;
    }

    const std::string& GetEventName(EventId eventId)
    {
        static const auto DEFAULT_NAME = std::string("None");

        auto it = EVENT_NAMES.find(eventId);
        if (it == EVENT_NAMES.end())
        {
            return DEFAULT_NAME;
        }

        const auto& [keyEventId, names] = *it;

        // Pick vendor-appropriate name.
        if (names.size() > 1)
        {
            int nameIdx = (int)GetGamepadVendorType();
            if (nameIdx < names.size())
            {
                return names[nameIdx];
            }
        }
        return names.front();
    }
}
