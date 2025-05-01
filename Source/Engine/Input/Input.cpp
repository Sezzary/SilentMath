#include "Framework.h"
#include "Engine/Input/Input.h"

#include "Engine/Input/Action.h"
#include "Engine/Input/Events.h"
#include "Utils/Utils.h"

using namespace Silent::Utils;

namespace Silent::Input
{
    void InputManager::Initialize()
    {
        _events.States.resize((int)EventId::Count);
        //_actionMap.reserve((int)ActionId::Count);
        _controlAxes.resize((int)ControlAxisId::Count);
    }

    void InputManager::Deinitialize()
    {
        
    }

    void InputManager::Update()
    {
        // Update event states.
        int eventStateIdx = 0;
        ReadKeyboard(eventStateIdx);
        ReadMouse(eventStateIdx);
        ReadController(eventStateIdx);
    }

    void InputManager::ReadKeyboard(int& eventStateIdx)
    {
        int         keyboardStateCount = 0;
        const bool* keyboardState      = SDL_GetKeyboardState(&keyboardStateCount);
        auto        modState           = SDL_GetModState();

        // Set keyboard key event states.
        for (auto scanCode : VALID_KEYBOARD_SCAN_CODES)
        {
            if (scanCode < keyboardStateCount)
            {
                _events.States[eventStateIdx] = keyboardState[scanCode] ? 1.0f : 0.0f;
            }

            eventStateIdx++;
        }

        // Set keyboard modifier event states.
        for (int modCode : VALID_KEYBOARD_MOD_CODES)
        {
            _events.States[eventStateIdx] = (modState & modCode) ? 1.0f : 0.0f;
            eventStateIdx++;
        }
    }

    void InputManager::ReadMouse(int& eventStateIdx)
    {
        auto pos      = Vector2::Zero;
        auto butState = SDL_GetMouseState(&pos.x, &pos.y);
        
        // Set mouse button event states.
        for (int butCode : VALID_MOUSE_BUT_CODES)
        {
            _events.States[eventStateIdx] = (butState & SDL_BUTTON_MASK(butCode)) ? 1.0f : 0.0f;
            eventStateIdx++;
        }

        // Set mouse position state.
        _events.PrevMousePosition = _events.MousePosition;
        _events.MousePosition     = pos;

        // TODO: Need to adjust for sensitivity.
        // Set camera axis. NOTE: Gamepad takes priority over keyboard/mouse.
        //_controlAxes[(int)ControlAxisId::Camera] = _events.PrevMousePosition - events.MousePosition;
    }

    void InputManager::ReadController(int& eventStateIdx)
    {
        constexpr float AXIS_DEADZONE = ((float)SHRT_MAX / 6.0f) / (float)SHRT_MAX;

        auto* gamepad = SDL_OpenGamepad(0);

        // Set gamepad button event states.
        for (auto butCode : VALID_GAMEPAD_BUT_CODES)
        {
            if (gamepad != nullptr)
            {
                _events.States[eventStateIdx] = SDL_GetGamepadButton(gamepad, butCode) ? 1.0f : 0.0f;
            }

            eventStateIdx++;
        }

        // Set gamepad stick axis event states.
        int i = 0;
        for (auto axisCode : VALID_GAMEPAD_STICK_AXIS_CODES)
        {
            if (gamepad != nullptr)
            {
                float val = (float)SDL_GetGamepadAxis(gamepad, axisCode) / (float)SHRT_MAX;

                _events.States[eventStateIdx + i]       = (val <= -AXIS_DEADZONE) ? abs(val) : 0.0f;
                _events.States[eventStateIdx + (i + 1)] = (val >=  AXIS_DEADZONE) ? abs(val) : 0.0f;

                // Set control axes. NOTE: Gamepad takes priority over keyboard/mouse.
                if (((i + 1) % 2) != 0)
                {
                    _controlAxes[i].x = val;
                }
                else
                {
                    _controlAxes[i].y = val;
                }
            }

            i++;
            eventStateIdx++;
        }
        eventStateIdx += VALID_GAMEPAD_STICK_AXIS_CODES.size();

        // Set gamepad trigger axis event states.
        for (auto axisCode : VALID_GAMEPAD_TRIG_AXIS_CODES)
        {
            if (gamepad != nullptr)
            {
                float val = (float)SDL_GetGamepadAxis(gamepad, axisCode) / (float)SHRT_MAX;

                _events.States[eventStateIdx] = (val >= AXIS_DEADZONE) ? val : 0.0f;
            }

            eventStateIdx++;
        }

        // Account for deadzone in control axes.
        for (auto& axis : _controlAxes)
        {
            if (axis.Length() < AXIS_DEADZONE)
            {
                axis = Vector2::Zero;
            }
        }
        
        SDL_CloseGamepad(gamepad);
    }
}
