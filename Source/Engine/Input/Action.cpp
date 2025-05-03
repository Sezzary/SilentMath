#include "Framework.h"
#include "Engine/Input/Action.h"

#include "Engine/Time.h"

namespace Silent::Input
{
    extern const std::vector<ActionId> GENERAL_ACTION_IDS
    {
        In::Up,
        In::Down,
        In::Left,
        In::Right
    };

    extern const std::vector<ActionId> MENU_ACTION_IDS
    {
        In::Enter,
        In::Cancel,
        In::Skip
    };

    extern const std::vector<ActionId> GAME_ACTION_IDS
    {
        In::Action,
        In::Aim,
        In::Light,
        In::Run,
        In::View,
        In::StepLeft,
        In::StepRight,
        In::Pause,
        In::Item,
        In::Map,
        In::Option
    };

    Action::Action(ActionId actionId)
    {
        _id = actionId;
    }

    ActionId Action::GetId() const
    {
        return _id;
    }
    
    float Action::GetValue() const
    {
        return _value;
    }
    
    uint Action::GetSecActive() const
    {
        return TicksToSec(_ticksActive);
    }
    
    uint Action::GetSecInactive() const
    {
        return TicksToSec(_ticksInactive);
    }
    
    bool Action::IsClicked(float valMin) const
    {
        return _value > valMin && _prevValue <= valMin;
    }

    bool Action::IsHeld(float delaySec, float valMin) const
    {
        if (_value <= valMin)
        {
            return false;
        }

        uint delayTicks = (delaySec == 0.0f) ? 0 : SecToTicks(delaySec);
        return _ticksActive >= delayTicks;
    }

    // NOTE: To avoid stutter on second pulse, ensure `initialDelaySec` is `delaySec` multiple.
    bool Action::IsPulsed(float delaySec, float initialDelaySec, float valMin) const
    {
        if (IsClicked(valMin))
        {
            return true;
        }

        if (!IsHeld(valMin) || _prevTicksActive == 0 || _ticksActive == _prevTicksActive)
        {
            return false;
        }

        float activeDelaySec   = (_ticksActive > SecToTicks(initialDelaySec)) ? delaySec : initialDelaySec;
        uint  activeDelayTicks = SecToTicks(activeDelaySec);

        uint delayTicks     = (uint)floor(_ticksActive / activeDelayTicks) * activeDelayTicks;
        uint prevDelayTicks = (uint)floor(_prevTicksActive / activeDelayTicks) * activeDelayTicks;
        return delayTicks > prevDelayTicks;
    }

    bool Action::IsReleased(float delaySecMax, float valMin) const
    {
        uint delayTicksMax = (delaySecMax == INFINITY) ? UINT_MAX : SecToTicks(delaySecMax);
        return _value <= valMin && _prevValue > valMin && _ticksActive <= delayTicksMax;
    }

    void Action::Update(bool val)
    {
        Update(val ? 1.0f : 0.0f);
    }

    void Action::Update(float val)
    {
        _prevValue = _value;
        _value     = val;

        if (IsClicked())
        {
            _prevTicksActive = 0;
            _ticksActive     = 0;
            _ticksInactive++;
        }
        else if (IsReleased())
        {
            _prevTicksActive = _ticksActive;
            _ticksInactive   = 0;
            _ticksActive++;
        }
        else if (IsHeld())
        {
            _prevTicksActive = _ticksActive;
            _ticksInactive   = 0;
            _ticksActive++;
        }
        else
        {
            _prevTicksActive = 0;
            _ticksActive     = 0;
            _ticksInactive++;
        }
    }

    void Action::Clear()
    {
        _value           = 0.0f;
        _prevValue       = 0.0f;
        _ticksActive     = 0;
        _prevTicksActive = 0;
        _ticksInactive   = 0;
    }
}
