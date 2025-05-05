#pragma once

#include "Engine/Input/Action.h"
#include "Engine/Input/Bindings.h"

namespace Silent { struct SettingsData; }

using namespace Silent;

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
        std::vector<float> States            = {}; // Index = `EventId`.
        Vector2            MousePosition     = {};
        Vector2            PrevMousePosition = {};
    };

    class InputManager
    {
    private:
        // Fields

        BindingManager                       _bindings    = {};
        EventData                            _events      = {};
        RumbleData                           _rumble      = {};
        std::unordered_map<ActionId, Action> _actions     = {};
        std::vector<Vector2>                 _controlAxes = {}; // Index = `ControlAxisId`.

    public:
        // Constructors

        InputManager() = default;

        // Inquirers

        bool IsActionClicked(ActionId actionId, float valMin = 0.0f) const;
        bool IsActionHeld(ActionId actionId, float delaySec = 0.0f, float valMin = 0.0f) const;
        bool IsActionPulsed(ActionId actionId, float delaySec, float initialDelaySec = 0.0f, float valMin = 0.0f) const;
        bool IsActionReleased(ActionId actionId, float delaySecMax = INFINITY, float valMin = 0.0f) const;

        // Utilities

        void Initialize(const SettingsData& settings);
        void Deinitialize();
        void Update(SDL_Window& window, const Vector2& wheelAxis);

        void Rumble(float power, float durationSec) const;

    private:
        // Helpers

        void ReadKeyboard(int& eventStateIdx);
        void ReadMouse(int& eventStateIdx, SDL_Window& window, const Vector2& wheelAxis);
        void ReadGamepad(int& eventStateIdx);

        void UpdateActions();
    };
}
