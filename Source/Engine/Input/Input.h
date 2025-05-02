#pragma once

namespace Silent::Input
{
    class      Action;
    enum class ActionId;

    enum class ControlAxisId
    {
        Move,
        Camera,

        Count
    };

	enum class RumbleMode
	{
		Left,
		Right,
		Dual
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
        //std::unordered_map<ActionId, Action> _actionMap   = {};
        std::vector<Vector2>                 _controlAxes = {}; // Index = `ControlAxisId`.

    public:
        // Constructors

        InputManager() = default;

        // Utilities

        void Initialize();
        void Deinitialize();
        void Update();

        void Rumble(float power, float durationSec, RumbleMode mode) const;

    private:
        // Helpers

        void ReadKeyboard(int& eventStateIdx);
        void ReadMouse(int& eventStateIdx);
        void ReadController(int& eventStateIdx);
    };
}
