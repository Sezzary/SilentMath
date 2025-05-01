#pragma once

namespace Silent::Input
{
    typedef enum class ActionId
    {
        Forward,

        Count
    } In;

    class Action
    {
    private:
        // Fields
        
        ActionId _id              = In::Forward;
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

        ActionId	 GetId() const;
        float		 GetValue() const;
        unsigned int GetTimeActive() const;
        unsigned int GetTimeInactive() const;
        
        // Inquirers

        bool IsClicked() const;
        bool IsHeld(float delaySecs = 0.0f) const;
        bool IsPulsed(float delaySecs, float initialDelaySecs = 0.0f) const;
        bool IsReleased(float delaySecsMax = INFINITY) const;

        // Utilities

        void Update(bool value);
        void Update(float value);
        void Clear();
    };
}
