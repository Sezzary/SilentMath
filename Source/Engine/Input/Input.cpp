#include "Framework.h"
#include "Engine/Input/Input.h"

#include "Engine/Input/Action.h"
#include "Engine/Input/Bindings.h"
#include "Engine/Input/Events.h"
#include "Engine/Input/Text.h"
#include "Engine/Services/Configuration.h"
#include "Engine/Services/Time.h"
#include "Utils/Utils.h"

using namespace Silent::Services;
using namespace Silent::Utils;

namespace Silent::Input
{
    InputManager g_Input = InputManager();

    const Action& InputManager::GetAction(ActionId actionId) const
    {
        return _actions.at(actionId);
    }

    void InputManager::SetRumble(RumbleMode mode, float intensityFrom, float intensityTo, float durationSec)
    {
        _rumble.Mode          = mode;
        _rumble.IntensityFrom = intensityFrom;
        _rumble.IntensityTo   = intensityTo;
        _rumble.DurationTicks =
        _rumble.Ticks         = SecToTicks(durationSec);
    }

    void InputManager::Initialize(const SettingsData& settings)
    {
        if (!SDL_Init(SDL_INIT_GAMEPAD))
        {
            Log("Failed to initialize gamepad subsystem: " + std::string(SDL_GetError()), LogLevel::Error);
        }

        // Initialize gamepad.
        _gamepad = SDL_OpenGamepad(0);

        // Initialize event states and control axes.
        _events.States.resize((int)EventId::Count);
        _controlAxes.resize((int)ControlAxisId::Count);

        // Initialize actions.
        _actions.reserve((int)ActionId::Count);
        for (int i = 0; i < (int)ActionId::Count; i++)
        {
            auto actionId = (ActionId)i;
            _actions.insert({ actionId, Action(actionId) });
        }

        // Initialize bindings.
        _bindings.Initialize(settings.KeyboardMouseBindings, settings.GamepadBindings);
    }

    void InputManager::Deinitialize()
    {
        SDL_CloseGamepad(_gamepad);
    }

    void InputManager::Update(SDL_Window& window, const SettingsData& settings, const Vector2& mouseWheelAxis)
    {
        if (!SDL_GamepadConnected(_gamepad))
        {
            HandleGamepadDisconnect();
        }

        // Capture event states.
        int eventStateIdx = 0;
        ReadKeyboard(eventStateIdx);
        ReadMouse(eventStateIdx, window, settings, mouseWheelAxis);
        ReadGamepad(eventStateIdx);

        // Update components.
        UpdateRumble();
        UpdateActions();
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

    void InputManager::ReadMouse(int& eventStateIdx, SDL_Window& window, const SettingsData& settings, const Vector2& wheelAxis)
    {
        auto pos      = Vector2::Zero;
        auto butState = SDL_GetMouseState(&pos.x, &pos.y);

        auto res = Vector2i::Zero;
        if (!SDL_GetWindowSize(&window, &res.x, &res.y))
        {
            Log("Failed to get window size: " + std::string(SDL_GetError()), LogLevel::Error);
        }

        // Set mouse button event states.
        for (int butCode : VALID_MOUSE_BUT_CODES)
        {
            _events.States[eventStateIdx] = (butState & SDL_BUTTON_MASK(butCode)) ? 1.0f : 0.0f;
            eventStateIdx++;
        }

        // Set mouse scroll events.
        _events.States[eventStateIdx]     = (wheelAxis.x < 0.0f) ? std::clamp(abs(wheelAxis.x), 0.0f, 1.0f) : 0.0f;
        _events.States[eventStateIdx + 1] = (wheelAxis.x > 0.0f) ? std::clamp(abs(wheelAxis.x), 0.0f, 1.0f) : 0.0f;
        _events.States[eventStateIdx + 2] = (wheelAxis.y < 0.0f) ? std::clamp(abs(wheelAxis.y), 0.0f, 1.0f) : 0.0f;
        _events.States[eventStateIdx + 3] = (wheelAxis.y > 0.0f) ? std::clamp(abs(wheelAxis.y), 0.0f, 1.0f) : 0.0f;

        eventStateIdx += SQUARE(Vector2::AXIS_COUNT);

        // Set mouse position state.
        _events.PrevMousePosition = _events.MousePosition;
        _events.MousePosition     = pos;

        auto  axis        = (_events.PrevMousePosition / res.ToVector2()) / (_events.MousePosition / res.ToVector2());
        float sensitivity = (settings.MouseSensitivity * 0.1f) + 0.4f;
        axis              *= sensitivity;
        
        // Set mouse movement event states.
        _events.States[eventStateIdx]     = (axis.x < 0.0f) ? abs(axis.x) : 0.0f;
        _events.States[eventStateIdx + 1] = (axis.x > 0.0f) ? abs(axis.x) : 0.0f;
        _events.States[eventStateIdx + 2] = (axis.y < 0.0f) ? abs(axis.y) : 0.0f;
        _events.States[eventStateIdx + 3] = (axis.y > 0.0f) ? abs(axis.y) : 0.0f;

        eventStateIdx += SQUARE(Vector2::AXIS_COUNT);
        
        // Set camera axis. NOTE: Gamepad takes priority over keyboard/mouse.
        _controlAxes[(int)ControlAxisId::Camera] = axis;
    }

    void InputManager::ReadGamepad(int& eventStateIdx)
    {
        constexpr float AXIS_DEADZONE = ((float)SHRT_MAX / 6.0f) / (float)SHRT_MAX;

        // Set gamepad button event states.
        for (auto butCode : VALID_GAMEPAD_BUT_CODES)
        {
            float state = 0.0f;
            if (_gamepad != nullptr)
            {
                state = SDL_GetGamepadButton(_gamepad, butCode) ? 1.0f : 0.0f;
            }
            _events.States[eventStateIdx] = state;

            eventStateIdx++;
        }

        // Collect stick axes.
        auto stickAxes = std::vector<Vector2>(VALID_GAMEPAD_STICK_AXIS_CODES.size() / Vector2::AXIS_COUNT);
        int  j         = 0;
        for (int i = 0; i < VALID_GAMEPAD_STICK_AXIS_CODES.size(); i++)
        {
            if (_gamepad != nullptr)
            {
                auto  axisCode = VALID_GAMEPAD_STICK_AXIS_CODES[i];
                float state    = (float)SDL_GetGamepadAxis(_gamepad, axisCode) / (float)SHRT_MAX;

                auto& axis = stickAxes[j];
                if ((i % Vector2::AXIS_COUNT) == 0)
                {
                    axis.x = state;
                }
                else
                {
                    axis.y = state;
                    j++;

                    // Remap to range.
                    if (axis.Length() >= AXIS_DEADZONE)
                    {
                        float remappedLength = Remap(axis.Length(), AXIS_DEADZONE, 1.0f, 0.0f, 1.0f);
                        axis = Vector2::Normalize(axis) * remappedLength;
                    }
                    else
                    {
                        axis = Vector2::Zero;
                    }
                }
            }
        }

        // Set gamepad stick axis event states and control axes.
        for (int i = 0; i < stickAxes.size(); i++)
        {
            const auto& axis = stickAxes[i];

            _events.States[eventStateIdx + i]       = (axis.x < 0.0f) ? abs(axis.x) : 0.0f;
            _events.States[eventStateIdx + (i + 1)] = (axis.x > 0.0f) ? abs(axis.x) : 0.0f;
            _events.States[eventStateIdx + (i + 2)] = (axis.y < 0.0f) ? abs(axis.y) : 0.0f;
            _events.States[eventStateIdx + (i + 3)] = (axis.y > 0.0f) ? abs(axis.y) : 0.0f;
            _controlAxes[i]                         = axis;

            eventStateIdx += Vector2::AXIS_COUNT * 2;
        }
        
        // Set camera axis. NOTE: Gamepad takes priority over keyboard/mouse.
        _controlAxes[(int)ControlAxisId::Camera] = stickAxes.front();

        // Set gamepad trigger axis event states.
        for (auto axisCode : VALID_GAMEPAD_TRIG_AXIS_CODES)
        {
            float state = 0.0f;
            if (_gamepad != nullptr)
            {
                // Remap state.
                state = (float)SDL_GetGamepadAxis(_gamepad, axisCode) / (float)SHRT_MAX;
                if (state >= AXIS_DEADZONE)
                {
                    state = Remap(state, AXIS_DEADZONE, 1.0f, 0.0f, 1.0f);
                }
            }
            _events.States[eventStateIdx] = state;

            eventStateIdx++;
        }
    }

    void InputManager::UpdateRumble()
    {
        if (_rumble.Ticks == 0)
        {
            return;
        }

        if (_gamepad == nullptr)
        {
            _rumble = {};
            return;
        }

        // Compute intensity.
        float alpha     = (float)_rumble.Ticks / (float)_rumble.DurationTicks;
        float intensity = std::lerp(_rumble.IntensityFrom, _rumble.IntensityTo, alpha);

        // Compute frequencies.
        ushort freqLow  = (_rumble.Mode == RumbleMode::Low  || _rumble.Mode == RumbleMode::LowAndHigh) ? (ushort)(intensity * USHRT_MAX) : 0;
        ushort freqHigh = (_rumble.Mode == RumbleMode::High || _rumble.Mode == RumbleMode::LowAndHigh) ? (ushort)(intensity * USHRT_MAX) : 0;

        // Compute duration.
        uint durationMs = (uint)round(TicksToSec(_rumble.DurationTicks) * 1000);
        
        // Rumble gamepad.
        if (!SDL_RumbleGamepad(_gamepad, freqLow, freqHigh, durationMs))
        {
            Log("Failed to rumble gamepad: " + std::string(SDL_GetError()), LogLevel::Error);
        }

        // Clear if rumble is complete.
        _rumble.Ticks--;
        if (_rumble.Ticks == 0)
        {
            _rumble = {};
        }
    }

    void InputManager::UpdateActions()
    {
        const auto& settings = g_Config.GetSettings();

        // 1) Update user action states.
        for (auto& [keyActionId, action] : _actions)
        {
            float eventState = 0.0f;

            // 1.1) Get highest gamepad event state.
            if (_gamepad != nullptr)
            {
                auto eventIds = _bindings.GetBoundEventIds(settings.ActiveKeyboardMouseProfileId, keyActionId);
                for (const auto& eventId : eventIds)
                {
                    eventState = std::max(eventState, _events.States[(int)eventId]);
                }
            }

            // 1.2) If no gamepad event state, get highest keyboard/mouse event state.
            if (eventState == 0.0f)
            {
                auto eventIds = _bindings.GetBoundEventIds(settings.ActiveGamepadProfileId, keyActionId);
                for (const auto& eventId : eventIds)
                {
                    eventState = std::max(eventState, _events.States[(int)eventId]);
                }
            }

            // 1.3) Update user action state.
            action.Update(eventState);
        }

        // 2) Update raw action states.
        for (auto profileId : RAW_EVENT_BINDING_PROFILE_IDS)
        {
            const auto& profile = _bindings.GetBindingProfile(profileId);
            for (auto& [keyActionId0, eventIds] : profile)
            {
                auto& action = _actions.at(keyActionId0);
                
                // 1.1) Get highest raw event state.
                float eventState = 0.0f;
                for (auto eventId : eventIds)
                {
                    eventState = std::max(eventState, _events.States[(int)eventId]);
                }

                // 2.2) Update raw action state.
                action.Update(eventState);
            }
        }
    }

    void InputManager::HandleGamepadDisconnect()
    {
        constexpr auto GAMEPAD_RECONNECT_INTERVAL_SEC = 2.0f;

        // Deinitialize disconnected gamepad.
        if (_gamepad != nullptr)
        {
            SDL_CloseGamepad(_gamepad);
            _gamepad = nullptr;

            Log("Gamepad disconnected.");
        }
        // Attempt to reinitialize gamepad intermittently.
        else
        {
            if (g_Time.TestInterval(SecToTicks(GAMEPAD_RECONNECT_INTERVAL_SEC)))
            {
                _gamepad = SDL_OpenGamepad(0);
                if (_gamepad != nullptr)
                {
                    Log("Gamepad reconnected.");
                }
            }
        }
    }
}
