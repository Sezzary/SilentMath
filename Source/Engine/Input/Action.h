#pragma once

namespace Silent::Input
{
    typedef enum class ActionId
    {
        // General

        Up,
        Down,
        Left,
        Right,

        // Menu

        Enter,
        Cancel,
        Skip,

        // Game
        
        Action,
        Aim,
        Light,
        Run,
        View,
        StepLeft,
        StepRight,
        Pause,
        Item,
        Map,
        Option,

        Count
    } In;

    extern const std::vector<ActionId> GENERAL_ACTION_IDS;
    extern const std::vector<ActionId> MENU_ACTION_IDS;
    extern const std::vector<ActionId> GAME_ACTION_IDS;

    class Action
    {
    private:
        // Fields
        
        ActionId _id              = In::Up;
        float    _state           = 0.0f;
        float    _prevState       = 0.0f;
        uint     _ticksActive     = 0;
        uint     _prevTicksActive = 0;
        uint     _ticksInactive   = 0;

    public:
        // Constructors

        Action() = default;
        Action(ActionId actionId);

        // Getters

        ActionId GetId() const;
        float    GetState() const;
        float    GetSecActive() const;
        float    GetSecInactive() const;
        
        // Inquirers
        
        bool IsClicked(float stateMin = 0.0f) const;
        bool IsHeld(float delaySec = 0.0f, float stateMin = 0.0f) const;
        bool IsPulsed(float delaySec, float initialDelaySec = 0.0f, float stateMin = 0.0f) const;
        bool IsReleased(float delaySecMax = INFINITY, float stateMin = 0.0f) const;

        // Utilities

        void Update(bool state);
        void Update(float state);
        void Clear();
    };
}
