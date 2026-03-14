#pragma once

namespace Silent::Renderer
{
    /** @brief GPU ping-pong render texture. */
    class PingPongTexture
    {
    private:
        // ==========
        // Constants
        // ==========

        static constexpr int TARGET_COUNT  = 2;

        // =======
        // Fields
        // =======

        SDL_GPUDevice*               _device   = nullptr;
        std::vector<SDL_GPUTexture*> _targets  = { nullptr, nullptr };
        int                          _writeIdx = 0;

    public:
        // =============
        // Constructors
        // =============

        /** @brief Creates a default uninitialized instance. */
        PingPongTexture() = default;

        /** @brief Gracefully destroys the instance and frees GPU resources. */
        ~PingPongTexture();

        // ========
        // Getters
        // ========

        /** @brief Gets the read texture.
         *
         * @return Read texture.
         */
        SDL_GPUTexture*& Read();

        /** @brief Gets the read texture.
         *
         * @return Read texture.
         */
        SDL_GPUTexture*& Write();

        // ==========
        // Utilities
        // ==========

        /** @brief Initializes the ping-pong texture with a reference to a GPU.
         *
         * @param device GPU device.
         */
        void Initialize(SDL_GPUDevice& device);

        /** @brief Swaps the internal texture targets. */
        void Swap();

        /** @brief Releases GPU texture resources. */
        void Release();
    };
}
