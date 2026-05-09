#pragma once

namespace Silent::Services
{
    /** @brief Toast data. */
    struct Toast
    {
        std::string Message = {};
        Color       Col     = Color::Clear;
        int         Life    = 0;
    };

    /** @brief Popup message toaster. */
    class ToastManager
    {
    private:
        // ==========
        // Constants
        // ==========

        static constexpr int   TOAST_COUNT_MAX       = 4;
        static constexpr float LIFE_SEC_MAX          = 6.0f;
        static constexpr float LIFE_SEC_START_FADING = 0.5f;

        // =======
        // Fields
        // =======

        std::vector<Toast> _toasts = {}; /** Active toasts. */

    public:
        // =============
        // Constructors
        // =============

        /** @brief Creates a default instance. */
        ToastManager() = default;

        // ==========
        // Utilities
        // ==========

        /** @brief Updates all toasts. */
        void Update();

        /** @brief Adds a new toast.
         *
         * @param msg Message to display.
         * @param color Message color.
         */
        void Add(const std::string& msg, const Color& color = Color::White);

    private:
        // ========
        // Helpers
        // ========

        /** @brief Submits all toasts for rendering. */
        void Render();
    };
}
