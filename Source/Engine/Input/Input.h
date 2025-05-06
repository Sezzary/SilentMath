#pragma once

#include "Engine/Input/Action.h"
#include "Engine/Input/Bindings.h"

namespace Silent::Services { struct SettingsData; }

using namespace Silent::Services;

namespace Silent::Input
{
    enum class ControlAxisId
    {
        Move,
        Camera,

        Count
    };

    enum class RumbleMode
    {
        Low,
        High,
        LowAndHigh
    };

    struct EventData
    {
        std::vector<float> States            = {}; // Index = `EventId`.
        Vector2            MousePosition     = {};
        Vector2            PrevMousePosition = {};
    };

    struct RumbleData
    {
        RumbleMode Mode          = RumbleMode::Low;
        float      IntensityFrom = 0.0f;
        float      IntensityTo   = 0.0f;
        uint       DurationTicks = 0;
        uint       Ticks         = 0;
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

        // Getters

        const Action& GetAction(ActionId actionId) const;

        // Setters

        void SetRumble(RumbleMode mode, float intensityFrom, float intensityTo, float durationSec);

        // Utilities

        void Initialize(const SettingsData& settings);
        void Deinitialize();
        void Update(SDL_Window& window, const SettingsData& settings, const Vector2& mouseWheelAxis);

    private:
        // Helpers

        void ReadKeyboard(int& eventStateIdx);
        void ReadMouse(int& eventStateIdx, SDL_Window& window, const SettingsData& settings, const Vector2& wheelAxis);
        void ReadGamepad(int& eventStateIdx, SDL_Gamepad* gamepad);

        void UpdateRumble(SDL_Gamepad* gamepad);
        void UpdateActions();
    };

    extern InputManager g_Input;
}
