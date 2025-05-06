#include "Framework.h"
#include "Engine/Services/Time.h"

#include "Utils/Utils.h"

using namespace Silent::Utils;

namespace Silent::Services
{
    TimeManager g_Time = TimeManager();

    float TimeManager::GetDeltaTime() const
    {
        uint64 uptimeMicrosec  = GetUptimeMicrosec();
        uint64 elapsedMicrosec = uptimeMicrosec - _prevUptimeMicrosec;
        return (float)elapsedMicrosec / 1000000.0f;
    }

    uint TimeManager::GetTicks() const
    {
        return std::min(_ticks, TPS / 2);
    }

    bool TimeManager::TestInterval(uint intervalTicks, uint offsetTicks) const
    {
        if (offsetTicks >= intervalTicks)
        {
            offsetTicks = 0;
            Log("Attempted to test time interval with offset greater than or equal to interval.");
        }
        
        uint64 ticks = GetUptimeMicrosec() / TICK_INTERVAL_MICROSEC;
        return (ticks % intervalTicks) == offsetTicks;
    }

    void TimeManager::Reset()
    {
        _baseMicrosec = GetEpochMicrosec();
    }

    void TimeManager::Update()
    {
        uint64 uptimeMicrosec  = GetUptimeMicrosec();
        uint64 elapsedMicrosec = uptimeMicrosec - _prevUptimeMicrosec;

        // Calculate ticks for elapsed period.
        _ticks = (uint)(elapsedMicrosec / TICK_INTERVAL_MICROSEC);

        // Set previous uptime if new ticks accumulated.
        if (_ticks != 0)
        {
            uint consumedTime = _ticks * TICK_INTERVAL_MICROSEC;
            _prevUptimeMicrosec = uptimeMicrosec;
        }
    }

    void TimeManager::WaitForNextTick() const
    {
        uint64 uptimeMicrosec  = GetUptimeMicrosec();
        uint64 elapsedMicrosec = uptimeMicrosec - _prevUptimeMicrosec;

        // Sleep current thread for remaining time before next tick.
        uint64 remainingMicrosec = TICK_INTERVAL_MICROSEC - (elapsedMicrosec % TICK_INTERVAL_MICROSEC);
        if (remainingMicrosec > 0 && remainingMicrosec < TICK_INTERVAL_MICROSEC)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(remainingMicrosec));
        }
    }

    uint64 TimeManager::GetUptimeMicrosec() const
    {
        return GetEpochMicrosec() - _baseMicrosec;
    }

    uint64 TimeManager::GetEpochMicrosec() const
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }

    uint SecToTicks(float sec)
    {
        constexpr float SEC_PER_TICK = 1.0f / (float)g_Time.TPS;

        return (uint)round(sec / SEC_PER_TICK);
    }

    float TicksToSec(uint ticks)
    {
        return (float)ticks / (float)g_Time.TPS;
    }
}
