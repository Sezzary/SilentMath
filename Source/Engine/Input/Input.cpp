#include "Framework.h"
#include "Engine/Input/Input.h"

#include "Engine/Application.h"
#include "Engine/Input/Action.h"
#include "Engine/Input/Binding.h"
#include "Engine/Input/Event.h"
#include "Engine/Input/Text.h"
#include "Engine/Services/Configuration.h"
#include "Engine/Services/Time.h"
#include "Utils/Parallel.h"
#include "Utils/Utils.h"

using namespace Silent::Services;
using namespace Silent::Utils;

namespace Silent::Input
{
    const Action& InputManager::GetAction(ActionId actionId) const
    {
        return _actions.at(actionId);
    }

    const Vector2& InputManager::GetAnalogAxis(AnalogAxisId axisId) const
    {
        return _analogAxes[(int)axisId];
    }

    const Vector2& InputManager::GetCursorPosition() const
    {
        return _events.CursorPosition;
    }

    const std::string& InputManager::GetText(const std::string& textId) const
    {
        return _text.GetText(textId);
    }

    std::vector<std::string> InputManager::GetTextLines(const std::string& bufferId, uint low, uint high) const
    {
        return _text.GetTextLines(bufferId, low, high);
    }

    uint InputManager::GetTextCursorPosition(const std::string& textId) const
    {
        return _text.GetCursorPosition(textId);
    }

    void InputManager::SetRumble(RumbleMode mode, float intensityFrom, float intensityTo, float durationSec)
    {
        _rumble.Mode          = mode;
        _rumble.IntensityFrom = intensityFrom;
        _rumble.IntensityTo   = intensityTo;
        _rumble.DurationTicks =
        _rumble.Ticks         = SEC_TO_TICK(durationSec);
    }

    bool InputManager::IsGamepadConnected() const
    {
        return _gamepad != nullptr;
    }

    bool InputManager::IsUsingGamepad() const
    {
        return _events.IsUsingGamepad;
    }

    void InputManager::Initialize()
    {
        const auto& options = g_App.GetConfig().GetOptions();

        if (!SDL_Init(SDL_INIT_GAMEPAD))
        {
            Log("Failed to initialize gamepad subsystem: " + std::string(SDL_GetError()), LogLevel::Error);
        }

        // Initialize gamepad.
        _gamepad = SDL_OpenGamepad(GAMEPAD_ID);
        if (_gamepad != nullptr)
        {
            Log("Connected gamepad with vendor ID " + std::to_string(SDL_GetGamepadVendor(_gamepad)) + ".");

            SetRumble(RumbleMode::LowAndHigh, 1.0f, 0.0f, 0.5f);
        }

        // Initialize event states and control axes.
        _events.States.resize((int)EventId::Count);
        _analogAxes.resize((int)AnalogAxisId::Count);

        // Initialize actions.
        _actions.reserve((int)ActionId::Count);
        for (int i = 0; i < (int)ActionId::Count; i++)
        {
            auto actionId = (ActionId)i;
            _actions.insert({ actionId, Action(actionId) });
        }

        // Initialize bindings.
        _bindings.Initialize(options.KeyboardMouseBindings, options.GamepadBindings);
    }

    void InputManager::Deinitialize()
    {
        SDL_CloseGamepad(_gamepad);
    }

    void InputManager::Update(SDL_Window& window, const Vector2& mouseWheelAxis)
    {
        if (!SDL_GamepadConnected(_gamepad))
        {
            HandleGamepadDisconnect();
        }

        // Capture event states.
        int eventStateIdx = 0;
        ReadKeyboard(eventStateIdx);
        ReadMouse(eventStateIdx, window, mouseWheelAxis);
        ReadGamepad(eventStateIdx);

        // Update components.
        UpdateRumble();
        UpdateActions();
        HandleHotkeyActions();
    }

    void InputManager::InsertText(const std::string& textId, uint lineWidthMax, uint charCountMax)
    {
        _text.InsertBuffer(textId, lineWidthMax, charCountMax);
    }
    
    void InputManager::UpdateText(const std::string& textId)
    {
        _text.UpdateBuffer(textId, _actions);
    }

    void InputManager::RemoveText(const std::string& textId)
    {
        _text.RemoveBuffer(textId);
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
                bool state = keyboardState[scanCode];
                if (state)
                {
                    _events.IsUsingGamepad = false;
                }

                _events.States[eventStateIdx] = state ? 1.0f : 0.0f;
            }

            eventStateIdx++;
        }

        // Set keyboard modifier event states.
        for (int modCode : VALID_KEYBOARD_MODIFIER_CODES)
        {
            bool state = modState & modCode;
            if (state)
            {
                _events.IsUsingGamepad = false;
            }

            _events.States[eventStateIdx] = state ? 1.0f : 0.0f;
            eventStateIdx++;
        }
    }

    void InputManager::ReadMouse(int& eventStateIdx, SDL_Window& window, const Vector2& wheelAxis)
    {
        const auto& options  = g_App.GetConfig().GetOptions();
        const auto& renderer = g_App.GetRenderer();

        auto pos      = Vector2::Zero;
        auto butState = SDL_GetMouseState(&pos.x, &pos.y);
        pos           = (pos / renderer.GetScreenResolution().ToVector2()) * SCREEN_SPACE_RES;

        // Set mouse button event states.
        for (int butCode : VALID_MOUSE_BUTTON_CODES)
        {
            bool state = butState & SDL_BUTTON_MASK(butCode);
            if (state)
            {
                _events.IsUsingGamepad = false;
            }

            _events.States[eventStateIdx] = state ? 1.0f : 0.0f;
            eventStateIdx++;
        }

        if (wheelAxis != Vector2::Zero)
        {
            _events.IsUsingGamepad = false;
        }

        // Set mouse scroll event states.
        _events.States[eventStateIdx]     = (wheelAxis.x < 0.0f) ? std::clamp(abs(wheelAxis.x), 0.0f, 1.0f) : 0.0f;
        _events.States[eventStateIdx + 1] = (wheelAxis.x > 0.0f) ? std::clamp(abs(wheelAxis.x), 0.0f, 1.0f) : 0.0f;
        _events.States[eventStateIdx + 2] = (wheelAxis.y < 0.0f) ? std::clamp(abs(wheelAxis.y), 0.0f, 1.0f) : 0.0f;
        _events.States[eventStateIdx + 3] = (wheelAxis.y > 0.0f) ? std::clamp(abs(wheelAxis.y), 0.0f, 1.0f) : 0.0f;
        eventStateIdx                    += SQUARE(Vector2::AXIS_COUNT);

        // Set mouse position state.
        _events.PrevCursorPosition = _events.CursorPosition;
        _events.CursorPosition     = pos;

        auto res = Vector2i::Zero;
        if (!SDL_GetWindowSize(&window, &res.x, &res.y))
        {
            Log("Failed to get window size: " + std::string(SDL_GetError()), LogLevel::Error);
        }
        
        float sensitivity = (options.MouseSensitivity * 0.1f) + 0.4f;
        auto  moveAxis    = ((_events.CursorPosition - _events.PrevCursorPosition) / res.ToVector2()) * sensitivity; // TODO
        if (moveAxis != Vector2::Zero)
        {
            _events.IsUsingGamepad = false;
        }

        // Set mouse movement event states.
        _events.States[eventStateIdx]     = (moveAxis.x < 0.0f) ? abs(moveAxis.x) : 0.0f;
        _events.States[eventStateIdx + 1] = (moveAxis.x > 0.0f) ? abs(moveAxis.x) : 0.0f;
        _events.States[eventStateIdx + 2] = (moveAxis.y < 0.0f) ? abs(moveAxis.y) : 0.0f;
        _events.States[eventStateIdx + 3] = (moveAxis.y > 0.0f) ? abs(moveAxis.y) : 0.0f;
        eventStateIdx                    += SQUARE(Vector2::AXIS_COUNT);

        // Set camera axis. NOTE: Right gamepad stick takes priority over mouse.
        _analogAxes[(int)AnalogAxisId::Camera] = moveAxis;

        // Set raw mouse axis.
        _analogAxes[(int)AnalogAxisId::Mouse] = moveAxis;
    }

    void InputManager::ReadGamepad(int& eventStateIdx)
    {
        constexpr float AXIS_DEADZONE = ((float)SHRT_MAX / 8.0f) / (float)SHRT_MAX;

        // Set gamepad button event states.
        for (auto butCode : VALID_GAMEPAD_BUTTON_CODES)
        {
            float state = 0.0f;
            if (_gamepad != nullptr)
            {
                state = SDL_GetGamepadButton(_gamepad, butCode);
            }
            if (state)
            {
                _events.IsUsingGamepad = true;
            }

            _events.States[eventStateIdx] = state ? 1.0f : 0.0f;
            eventStateIdx++;
        }

        // Collect stick axes.
        auto stickAxes = std::vector<Vector2>(VALID_GAMEPAD_STICK_AXIS_CODES.size() / Vector2::AXIS_COUNT);
        int  j         = 0;
        for (int i = 0; i < VALID_GAMEPAD_STICK_AXIS_CODES.size(); i++)
        {
            if (_gamepad == nullptr)
            {
                break;
            }

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

                // Remap axis to active range.
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

        // Set gamepad stick axis event states and control axes.
        for (int i = 0; i < stickAxes.size(); i++)
        {
            const auto& axis = stickAxes[i];
            if (axis != Vector2::Zero)
            {
                _events.IsUsingGamepad = true;
            }

            _events.States[eventStateIdx + i]       = (axis.x < 0.0f) ? abs(axis.x) : 0.0f;
            _events.States[eventStateIdx + (i + 1)] = (axis.x > 0.0f) ? abs(axis.x) : 0.0f;
            _events.States[eventStateIdx + (i + 2)] = (axis.y < 0.0f) ? abs(axis.y) : 0.0f;
            _events.States[eventStateIdx + (i + 3)] = (axis.y > 0.0f) ? abs(axis.y) : 0.0f;
            _analogAxes[i]                          = axis;
            eventStateIdx                          += Vector2::AXIS_COUNT * 2;
        }

        // Set camera axis. NOTE: Right gamepad stick takes priority over mouse.
        if (stickAxes.back() != Vector2::Zero)
        {
            _analogAxes[(int)AnalogAxisId::Camera] = stickAxes.back();
        }

        // Set raw gamepad stick axes.
        _analogAxes[(int)AnalogAxisId::StickLeft]  = stickAxes.front();
        _analogAxes[(int)AnalogAxisId::StickRight] = stickAxes.back();

        // Set gamepad trigger axis event states.
        for (auto axisCode : VALID_GAMEPAD_TRIGGER_AXIS_CODES)
        {
            float state = 0.0f;
            if (_gamepad != nullptr)
            {
                // Remap state to active range.
                state = (float)SDL_GetGamepadAxis(_gamepad, axisCode) / (float)SHRT_MAX;
                if (state >= AXIS_DEADZONE)
                {
                    state = Remap(state, AXIS_DEADZONE, 1.0f, 0.0f, 1.0f);
                }
            }
            if (state > 0.0f)
            {
                _events.IsUsingGamepad = true;
            }

            _events.States[eventStateIdx] = state;
            eventStateIdx++;
        }
    }

    void InputManager::UpdateRumble()
    {
        if (_rumble.Ticks == 0 || _gamepad == nullptr)
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
        uint durationMs = (uint)round(TICK_TO_SEC(_rumble.DurationTicks) * 1000);

        // Rumble gamepad.
        if (!SDL_RumbleGamepad(_gamepad, freqLow, freqHigh, durationMs))
        {
            Log("Failed to rumble gamepad: " + std::string(SDL_GetError()), LogLevel::Error);
        }
    }

    void InputManager::UpdateActions()
    {
        const auto& options = g_App.GetConfig().GetOptions();

        // 1) Update user action states.
        auto updateUserActions = [&]()
        {
            // Get user action binding profiles.
            auto gamepadProfile = _bindings.GetBindingProfile(options.ActiveGamepadProfileId);
            auto kmProfile      = _bindings.GetBindingProfile(options.ActiveKeyboardMouseProfileId);

            for (auto actionGroupId : USER_ACTION_GROUP_IDS)
            {
                const auto& actionIds = ACTION_ID_GROUPS.at(actionGroupId);
                for (auto actionId : actionIds)
                {
                    auto& action = _actions.at(actionId);
                    float state  = 0.0f;

                    // Get max gamepad event state.
                    if (_gamepad != nullptr)
                    {
                        auto gamepadEventIds = gamepadProfile.at(actionId);
                        for (const auto& eventId : gamepadEventIds)
                        {
                            state = std::max(state, _events.States[(int)eventId]);
                        }
                    }

                    // If no valid gamepad event state, get max keyboard/mouse event state.
                    if (state == 0.0f)
                    {
                        auto kmEventIds = kmProfile.at(actionId);
                        for (const auto& eventId : kmEventIds)
                        {
                            state = std::max(state, _events.States[(int)eventId]);
                        }
                    }

                    // Use max bound event state.
                    action.Update(state);
                }
            }
        };

        // 2) Update raw action states.
        auto updateRawActions = [&]()
        {
            for (auto profileId : RAW_EVENT_BINDING_PROFILE_IDS)
            {
                const auto& profile = _bindings.GetBindingProfile(profileId);
                for (auto& [keyActionId, eventIds] : profile)
                {
                    auto& action = _actions.at(keyActionId);
                    float state  = 0.0f;

                    for (auto eventId : eventIds)
                    {
                        state = std::max(state, _events.States[(int)eventId]);
                    }

                    // Use max bound event state.
                    action.Update(state);
                }
            }
        };

        // Update action states asynchronously.
        auto tasks = ParallelTasks
        {
            [&]() { updateUserActions(); },
            [&]() { updateRawActions(); }
        };
        g_Parallel.AddTasks(tasks).wait();
    }

    void InputManager::HandleHotkeyActions()
    {
        // Capture screenshot.
        static bool dbScreenshot = true;
        if ((_events.States[(int)EventId::PrintScreen] || _events.States[(int)EventId::F12]) && dbScreenshot)
        {
            const auto& renderer = g_App.GetRenderer();
            renderer.SaveScreenshot();

            Log("Captured screenshot.", LogLevel::Info, LogMode::DebugRelease, true);
        }
        dbScreenshot = !(_events.States[(int)EventId::PrintScreen] || _events.States[(int)EventId::F12]);

        // Toggle fullscreen.
        static bool dbFullscreen = true;
        if (((_events.States[(int)EventId::Alt] && _events.States[(int)EventId::Return]) || _events.States[(int)EventId::F11]) && dbFullscreen)
        {
            g_App.ToggleFullscreen();

            Log("Toggled fullscreen.", LogLevel::Info, LogMode::DebugRelease, true);
        }
        dbFullscreen = !((_events.States[(int)EventId::Alt] && _events.States[(int)EventId::Return]) || _events.States[(int)EventId::F11]);

        auto& options = g_App.GetConfig().GetOptions();
        if (options.EnableDebugMode)
        {
            // Toggle debug GUI.
            static bool dbDebugGui = true;
            if (_events.States[(int)EventId::Grave] && dbDebugGui)
            {
                options.EnableDebugGui = !options.EnableDebugGui;
                g_DebugData.Page       = DebugPage::None;

                Log("Toggled debug mode.", LogLevel::Info, LogMode::DebugRelease, true);
            }
            dbDebugGui = !_events.States[(int)EventId::Grave];
        }
    }

    void InputManager::HandleGamepadDisconnect()
    {
        constexpr auto GAMEPAD_RECONNECT_INTERVAL_SEC = 2.0f;

        // Deinitialize disconnected gamepad.
        if (_gamepad != nullptr)
        {
            Log("Gamepad disconnected.");
            // TODO: Add toast notification.

            SDL_CloseGamepad(_gamepad);
            _gamepad = nullptr;
        }
        // Intermittently attempt reconnecting gamepad.
        else
        {
            if (g_Time.TestInterval(SEC_TO_TICK(GAMEPAD_RECONNECT_INTERVAL_SEC)))
            {
                _gamepad = SDL_OpenGamepad(GAMEPAD_ID);
                if (_gamepad != nullptr)
                {
                    Log("Connected gamepad with vendor ID " + std::to_string(SDL_GetGamepadVendor(_gamepad)) + ".");
                    // TODO: Add toast notification.

                    SetRumble(RumbleMode::LowAndHigh, 1.0f, 0.0f, 0.5f);
                }
            }
        }
    }
}
