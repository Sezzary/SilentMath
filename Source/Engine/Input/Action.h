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
        float    _value           = 0.0f;
        float    _prevValue       = 0.0f;
        ulong    _ticksActive     = 0;
        ulong    _prevTicksActive = 0;
        ulong    _ticksInactive   = 0;

    public:
        // Constructors

        Action() = default;
        Action(ActionId actionId);

        // Getters

        ActionId GetId() const;
        float	 GetValue() const;
        uint     GetSecActive() const;
        uint     GetSecInactive() const;
        
        // Inquirers
        
        bool IsClicked(float valMin = 0.0f) const;
        bool IsHeld(float delaySec = 0.0f, float valMin = 0.0f) const;
        bool IsPulsed(float delaySec, float initialDelaySec = 0.0f, float valMin = 0.0f) const;
        bool IsReleased(float delaySecMax = INFINITY, float valMin = 0.0f) const;

        // Utilities

        void Update(bool val);
        void Update(float val);
        void Clear();
    };
}
