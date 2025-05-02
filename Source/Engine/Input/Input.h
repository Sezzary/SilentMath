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
        // TODO
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

        EventData                            _events      = {};
        //std::unordered_map<ActionId, Action> _actionMap   = {};
        std::vector<Vector2>                 _controlAxes = {}; // Index = `ControlAxisId`.

    public:
        // Constructors

        InputManager() = default;

        // Utilities

        void Initialize();
        void Deinitialize();
        void Update();

        void Rumble() const;

    private:
        // Helpers

        void ReadKeyboard(int& eventStateIdx);
        void ReadMouse(int& eventStateIdx);
        void ReadController(int& eventStateIdx);
    };
}
