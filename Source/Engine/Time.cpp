#include "Framework.h"
#include "Engine/Time.h"

#include "Utils/Utils.h"

using namespace Silent::Utils;

namespace Silent
{
    TimeManager& g_Time = TimeManager::Get();

    TimeManager& TimeManager::Get()
    {
        static auto instance = TimeManager();
        return instance;
    }

    float TimeManager::GetDeltaTime() const
    {
        ulong uptimeMicrosecs = GetUptimeMicrosecs();
        ulong elapsedMicrosecs = uptimeMicrosecs - _prevUptimeMicrosecs;
        return (float)elapsedMicrosecs / 1000000.0f;
    }

    uint TimeManager::GetTicks() const
    {
        return std::min(_ticks, TPS / 2);
    }

    ulong TimeManager::GetUptimeMicrosecs() const
    {
        return GetEpochMicrosecs() - _baseMicrosecs;
    }

    bool TimeManager::TestInterval(uint intervalTicks, uint offsetTicks) const
    {
        if (offsetTicks >= intervalTicks)
        {
            offsetTicks = 0;
            Log("Attempted to test time interval with offset greater than or equal to interval.");
        }
        
        ulong ticks = GetUptimeMicrosecs() / TICK_INTERVAL_MICROSECS;
        return (ticks % intervalTicks) == offsetTicks;
    }

    void TimeManager::Reset()
    {
        _baseMicrosecs = GetEpochMicrosecs();
    }

    void TimeManager::Update()
    {
        ulong uptimeMicrosecs = GetUptimeMicrosecs();
        ulong elapsedMicrosecs = uptimeMicrosecs - _prevUptimeMicrosecs;

        // Calculate ticks for elapsed period.
        _ticks = (uint)(elapsedMicrosecs / TICK_INTERVAL_MICROSECS);

        // Set previous uptime if new ticks accumulated.
        if (_ticks != 0)
        {
            uint consumedTime = _ticks * TICK_INTERVAL_MICROSECS;
            _prevUptimeMicrosecs = uptimeMicrosecs;
        }
    }

    void TimeManager::WaitForNextTick() const
    {
        ulong uptimeMicrosecs = GetUptimeMicrosecs();
        ulong elapsedMicrosecs = uptimeMicrosecs - _prevUptimeMicrosecs;

        // Sleep current thread for remaining time before next tick.
        ulong remainingMicrosecs = TICK_INTERVAL_MICROSECS - (elapsedMicrosecs % TICK_INTERVAL_MICROSECS);
        if (remainingMicrosecs > 0 && remainingMicrosecs < TICK_INTERVAL_MICROSECS)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(remainingMicrosecs));
        }
    }

    ulong TimeManager::GetEpochMicrosecs() const
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }

    uint SecToTick(float sec)
    {
        return (uint)round(sec / (1.0f / (float)g_Time.TPS));
    }

    float TicksToSec(uint ticks)
    {
        return (float)ticks / (float)g_Time.TPS;
    }
}
