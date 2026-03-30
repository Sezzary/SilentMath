#pragma once

#include "Audio/Spu.h"

namespace Silent::Audio
{
    /** @brief Audio manager. */
    class AudioManager
    {
    private:
        // =======
        // Fields
        // =======

        SDL_AudioDeviceID _device = 0;
        SDL_AudioStream*  _stream = nullptr;
        VirtualSpu        _spu    = VirtualSpu();

    public:
        // =============
        // Constructors
        // =============

        AudioManager() = default;

        // ==========
        // Utilities
        // ==========

        /** @brief Initializes the audio stream. */
        void Initialize();

        /** @brief Gracefully deinitializes the audio manager. */
        void Deinitialize();

        /** @brief Updates the audio system. */
        void Update();
    };
}
