#pragma once

namespace Silent::Services
{
    constexpr int TICKS_PER_SECOND = 60;

    /** @brief Game clock manager. */
    class ClockManager
    {
    private:
        // ==========
        // Constants
        // ==========

        static constexpr int TICK_INTERVAL_DURATION = 1000000 / TICKS_PER_SECOND;

        // =======
        // Fields
        // =======

        int    _ticks              = 0;
        uint64 _prevUptimeDuration = 0;

    public:
        // =============
        // Constructors
        // =============

        /** @brief Creates a default uninitialized instance. */
        ClockManager() = default;

        // ========
        // Getters
        // ========

        /** @brief Gets the delta time in seconds since the previous tick.
         *
         * @return Delta time.
         */
        float GetDeltaTime() const;

        /** @brief Gets the accumulated ticks since the previous update.
         * If the main loop takes too long, it returns more than one tick to compensate.
         *
         * @return Accumulated ticks.
         */
        int GetTicks() const;

        // ==========
        // Inquirers
        // ==========

        /** @brief Tests if a tick interval has passed.
         *
         * @param intervalTicks Tick duration of the desired interval.
         * @param offsetTicks Tick offset.
         * @return `true` if the interval has passed, `false` otherwise.
         */
        bool TestInterval(int intervalTicks, int offsetTicks = 0) const;

        // ==========
        // Utilities
        // ==========

        /** @brief Initializes the clock manager. */
        void Initialize();

        /** @brief Updates the clock manager, setting accumulated ticks. */
        void Update();

        /** @brief Blocks the current thread until the next tick.
         * Used to avoid busy-waiting in the main loop if a logic iteration has finished early.
         */
        void WaitForNextTick() const;
    };

    /** @brief Converts seconds to ticks.
     *
     * @param sec Seconds to convert.
     * @return Seconds converted to ticks.
     */
    constexpr int SEC_TO_TICK(float sec)
    {
        sec = std::max(sec, 0.0f);
        return (sec == 0.0f) ? 0 : std::max((int)ROUND(sec * (float)TICKS_PER_SECOND), 1);
    }

    /** @brief Converts ticks to seconds.
     *
     * @param ticks Ticks to convert.
     * @return Ticks converted to seconds.
     */
    constexpr float TICK_TO_SEC(int ticks)
    {
        ticks = std::max(ticks, 0);
        return (float)ticks / (float)TICKS_PER_SECOND;
    }

    /** @brief Gets a string containing the current system date as `YYYY-MM-DD`.
     *
     * @return String containing the current system date.
     */
    std::string GetCurrentDateString();

    /** @brief Gets a string containing the current system time as `HH-MM-SS_mmm`.
     *
     * @return String containing the current system time.
     */
    std::string GetCurrentTimeString();
}
