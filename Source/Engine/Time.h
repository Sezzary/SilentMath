#pragma once

namespace Silent
{
    class TimeController
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

        TimeController()                           = default;
        TimeController(const TimeController& time) = delete;

    public:
        // Getters

        static TimeController& Get();

        float GetDeltaTime() const;
        uint  GetTicksPerSecond() const;
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
        
        TimeController& operator=(const TimeController& time) = delete;
    };

    extern TimeController& g_Time;
}
