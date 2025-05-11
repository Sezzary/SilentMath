#pragma once

#include "Engine/Input/Action.h"
#include "Engine/Input/Bindings.h"
#include "Engine/Input/Text.h"

namespace Silent::Services { struct SettingsData; }

using namespace Silent::Services;

namespace Silent::Input
{
    enum class ControlAxisId
    {
        Move,
        Camera, // TODO: Should be influenced by both right stick and mouse.

        Count
    };

    enum class RumbleMode
    {
        Low,
        High,
        LowAndHigh
    };

    struct Event
    {
        std::vector<float> States             = {}; // Index = `EventId`.
        Vector2            CursorPosition     = {};
        Vector2            PrevCursorPosition = {};
    };

    struct Rumble
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

        SDL_Gamepad* _gamepad = nullptr;

        TextManager                          _text        = TextManager();
        BindingManager                       _bindings    = BindingManager();
        Event                                _events      = {};
        Rumble                               _rumble      = {};
        std::unordered_map<ActionId, Action> _actions     = {};
        std::vector<Vector2>                 _controlAxes = {}; // Index = `ControlAxisId`.

    public:
        // Constructors

        InputManager() = default;

        // Getters

        const Action&      GetAction(ActionId actionId) const;
        const Vector2&     GetCursorPosition() const;
        const std::string& GetText(const std::string& textId) const;
        const uint         GetTextCursorPosition(const std::string& textId) const;
        
        // Setters

        void SetRumble(RumbleMode mode, float intensityFrom, float intensityTo, float durationSec);

        // Inquirers

        bool IsUsingGamepad() const;

        // Utilities

        void Initialize(const SettingsData& settings);
        void Deinitialize();
        void Update(SDL_Window& window, const SettingsData& settings, const Vector2& mouseWheelAxis);

        void UpdateText(const std::string& textId, uint lengthMax = UINT_MAX);
        void ClearText(const std::string& textId);

    private:
        // Helpers

        void ReadKeyboard(int& eventStateIdx);
        void ReadMouse(int& eventStateIdx, SDL_Window& window, const SettingsData& settings, const Vector2& wheelAxis);
        void ReadGamepad(int& eventStateIdx);

        void UpdateRumble();
        void UpdateActions();

        void HandleGamepadDisconnect();
    };

    extern InputManager g_Input;
}
