#pragma once

#include "Engine/Input/Action.h"
#include "Engine/Input/Bindings.h"

namespace Silent::Input
{
    enum class ControlAxisId
    {
        Move,
        Camera,

        Count
    };

    struct RumbleData
    {
        float PowerFrom     = 0.0f;
        float PowerTo       = 0.0f;
        uint  DurationTicks = 0;
    };

    struct EventData
    {
        std::vector<float> States        = {}; // Index = `EventId`.
        Vector2            MousePosition = {};
    };

    class InputManager
    {
    private:
        // Fields

        EventData                            _events      = {};
        RumbleData                           _rumble      = {};
        BindingManager                       _bindings    = {};
        std::unordered_map<ActionId, Action> _actions     = {};
        std::vector<Vector2>                 _controlAxes = {}; // Index = `ControlAxisId`.

        BindingProfileId _keyboardMouseProfileId = BindingProfileId::KeyboardMouseDefault0;
        BindingProfileId _gamepadProfileId       = BindingProfileId::GamepadDefault0;

    public:
        // Constructors

        InputManager() = default;

        // Inquirers

        bool IsActionClicked(ActionId actionId, float valMin = 0.0f) const;
        bool IsActionHeld(ActionId actionId, float delaySec = 0.0f, float valMin = 0.0f) const;
        bool IsActionPulsed(ActionId actionId, float delaySec, float initialDelaySec = 0.0f, float valMin = 0.0f) const;
        bool IsActionReleased(ActionId actionId, float delaySecMax = INFINITY, float valMin = 0.0f) const;

        // Utilities

        void Initialize();
        void Deinitialize();
        void Update();

        void Rumble(float power, float durationSec) const;

    private:
        // Helpers

        void ReadKeyboard(int& eventStateIdx);
        void ReadMouse(int& eventStateIdx);
        void ReadController(int& eventStateIdx);

        void UpdateActions();
    };
}
