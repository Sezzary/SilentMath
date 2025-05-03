#include "Framework.h"
#include "Engine/Input/Action.h"

#include "Engine/Time.h"

namespace Silent::Input
{
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

		uint delayTicks = (delaySec == 0.0f) ? 0 : SecToTick(delaySec);
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

        float activeDelaySec   = (_ticksActive > SecToTick(initialDelaySec)) ? delaySec : initialDelaySec;
        uint  activeDelayTicks = SecToTick(activeDelaySec);

        uint delayTicks     = (uint)floor(_ticksActive / activeDelayTicks) * activeDelayTicks;
        uint prevDelayTicks = (uint)floor(_prevTicksActive / activeDelayTicks) * activeDelayTicks;
        return delayTicks > prevDelayTicks;
    }

    bool Action::IsReleased(float delaySecMax, float valMin) const
    {
        uint delayTicksMax = (delaySecMax == INFINITY) ? UINT_MAX : SecToTick(delaySecMax);
        return _value <= valMin && _prevValue > valMin && _ticksActive <= delayTicksMax;
    }

    void Action::Update(bool val)
    {
        Update(val ? 1.0f : 0.0f);
    }

    void Action::Update(float val)
    {
        UpdateValue(val);

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

    void Action::UpdateValue(float val)
    {
		_prevValue = _value;
		_value     = val;
    }

    uint Action::SecToTick(float sec) const
    {
        return (uint)round(sec / (1.0f / (float)g_Time.TPS));
    }

    float Action::TicksToSec(uint ticks) const
    {
        return (float)ticks / (float)g_Time.TPS;
    }
}
