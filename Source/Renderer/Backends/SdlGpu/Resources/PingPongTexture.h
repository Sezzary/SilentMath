#pragma once

#include "Renderer/Common/Resources/PingPongTexture.h"

namespace Silent::Renderer
{
    /** @brief GPU ping-pong render target texture. */
    class PingPongTexture : public PingPongTextureBase
    {
    private:
        // =======
        // Fields
        // =======

        SDL_GPUDevice*                            _device  = nullptr;
        std::array<SDL_GPUTexture*, TARGET_COUNT> _targets = { nullptr, nullptr };

    public:
        // =============
        // Constructors
        // =============

        /** @brief Creates a default uninitialized instance. */
        PingPongTexture() = default;

        // ========
        // Getters
        // ========

        /** @brief Gets the active read GPU texture. Used as the main render texture.
         *
         * @return Read GPU texture.
         */
        SDL_GPUTexture*& Read();

        /** @brief Gets the active write GPU texture. Used as the main draw texture.
         *
         * @return Write GPU texture.
         */
        SDL_GPUTexture*& Write();

        // ===================
        // Override Inquirers
        // ===================

        bool IsValid() const override;

        // ==========
        // Utilities
        // ==========

        /** @brief Initializes the ping-pong texture with a reference to a GPU.
         *
         * @param device GPU device.
         */
        void Initialize(SDL_GPUDevice& device);

        // ===================
        // Override Utilities
        // ===================

        void Release() override;
    };
}
