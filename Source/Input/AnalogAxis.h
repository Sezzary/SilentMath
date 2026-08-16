#pragma once

namespace Silent::Input
{
    /** @brief Input analog axis IDs for specialized gameplay and raw device axes. */
    enum class AnalogAxisId
    {
        /** Gameplay */

        Move,
        Camera,

        /** Raw */

        Mouse,
        StickLeft,
        StickRight,

        Count
    };

    /** @brief Input analog axis group IDs. */
    enum class AnalogAxisGroupId
    {
        Gameplay,
        Raw,

        Recordable
    };

    extern const std::vector<std::vector<AnalogAxisId>> ANALOG_AXIS_ID_GROUPS; /** Index = `AnalogAxisGroupId`. */
    extern const std::vector<AnalogAxisGroupId>         USER_ANALOG_AXIS_GROUP_IDS;
    extern const std::vector<AnalogAxisGroupId>         RAW_ANALOG_AXIS_GROUP_IDS;

    /** @brief Input analog axis. */
    class AnalogAxis
    {
    private:
        // =======
        // Fields
        // =======

        AnalogAxisId _id       = AnalogAxisId::Move;
        bool         _isLocked = false;
        
    public:
        Vector2 State = Vector2::Zero;

        // =============
        // Constructors
        // =============

        /** @brief Creates a default uninitialized instance. */
        AnalogAxis() = default;

        /** @brief Creates an instance with a given ID.
         *
         * @param axisId Analog axis ID to set.
         */
        AnalogAxis(AnalogAxisId axisId);

        // ========
        // Getters
        // ========

        /** @brief Gets the analog axis' ID.
         *
         * @return Analog axis ID.
         */
        AnalogAxisId GetId() const;

        // ==========
        // Inquirers
        // ==========

        /** @brief Checks if the analog axis is locked from user input.
         *
         * @return `true` if locked, `false` otherwise.
         */
        bool IsLocked() const;

        // ==========
        // Utilities
        // ==========

        /** @brief Flags the analog axis as locked from user input. */
        void Lock();

        /** @brief Unflags the analog axis as locked from user input. */
        void Unlock();
    };
}
