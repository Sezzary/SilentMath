#pragma once

namespace Silent::Renderer
{
    /** @brief GPU ping-pong render target texture base. */
    class PingPongTextureBase
    {
    protected:
        // ==========
        // Constants
        // ==========

        static constexpr int TARGET_COUNT = 2;

        // =======
        // Fields
        // =======

        int _writeIdx = 0;

    public:
        // =============
        // Constructors
        // =============

        /** @brief Gracefully destroys the instance and frees GPU resources. */
        virtual ~PingPongTextureBase() = default;

        // ==========
        // Utilities
        // ==========

        /** @brief Swaps the internal texture targets. */
        void Swap();

        /** @brief Releases GPU texture resources. */
        virtual void Release() = 0;
    };
}
