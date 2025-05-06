#pragma once

namespace Silent::Services
{
    class TimeManager
    {
    public:
        // Constants

        static constexpr uint TPS = 60;

    private:
        static constexpr uint TICK_INTERVAL_MICROSEC = 1000000 / TPS;

        // Fields

        uint   _ticks              = 0;
        uint64 _prevUptimeMicrosec = 0;
        uint64 _baseMicrosec       = 0;

    public:
        // Constructors

        TimeManager() = default;

        // Getters

        float GetDeltaTime() const;
        uint  GetTicks() const;

        // Inquirers

        bool TestInterval(uint intervalTicks, uint offsetTicks = 0) const;

        // Utilities

        void Reset();
        void Update();
        void WaitForNextTick() const;

    private:
        // Helpers

        uint64 GetUptimeMicrosec() const;
        uint64 GetEpochMicrosec() const;
    };

    extern TimeManager g_Time;

    uint  SecToTicks(float sec);
    float TicksToSec(uint ticks);
}
