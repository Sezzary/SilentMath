#pragma once

namespace Silent
{
    class TimeController
    {
    public:
        // Constants

        static constexpr uint TPS = 60;

    private:
        // Fields

        uint  _ticks               = 0;
        ulong _prevUptimeMicrosecs = 0;
        ulong _initMicrosecs       = 0;

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

        void Initialize();
        void Update();
        void WaitForNextTick() const;

    private:
        // Helpers

        ulong GetEpochMicrosecs() const;
        uint  GetTickIntervalMicrosecs() const;

        // Operators
        
        TimeController& operator=(const TimeController& time) = delete;
    };

    extern TimeController& g_Time;
}
