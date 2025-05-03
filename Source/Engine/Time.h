#pragma once

namespace Silent
{
    class TimeManager
    {
    public:
        // Constants

        static constexpr uint TPS = 60;

    private:
        static constexpr uint TICK_INTERVAL_MICROSECS = 1000000 / TPS;

        // Fields

        uint  _ticks               = 0;
        ulong _prevUptimeMicrosecs = 0;
        ulong _baseMicrosecs       = 0;

        // Constructors

        TimeManager()                        = default;
        TimeManager(const TimeManager& time) = delete;

    public:
        // Getters

        static TimeManager& Get();

        float GetDeltaTime() const;
        uint  GetTicks() const;
        ulong GetUptimeMicrosecs() const;

        // Inquirers

        bool TestInterval(uint intervalTicks, uint offsetTicks = 0) const;

        // Utilities

        void Reset();
        void Update();
        void WaitForNextTick() const;

    private:
        // Helpers

        ulong GetEpochMicrosecs() const;

        // Operators
        
        TimeManager& operator=(const TimeManager& time) = delete;
    };

    extern TimeManager& g_Time;

    uint  SecToTick(float sec);
    float TicksToSec(uint ticks);
}
