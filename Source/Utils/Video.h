#pragma once

namespace Silent::Utils
{
    /** @brief MPEG1 video player. */
    class VideoPlayer
    {
    private:
        // =======
        // Fields
        // =======

        plm_t*             _plm         = nullptr;
        std::vector<byte>  _frameBuffer = {};
        std::vector<float> _audioBuffer = {};
        std::mutex         _audioMutex  = {};

        std::string           _activeVideoName = {};
        std::filesystem::path _videosPath      = {};

    public:
        // =============
        // Constructors
        // =============

        /** @brief Creates a default uninitialized instance. */
        VideoPlayer() = default;

        /** @brief Gracefully destroys the instance and frees resources. */
        ~VideoPlayer();

        // ========
        // Getters
        // ========

        /** @brief Gets the active video's pixel resolution.
         *
         * @return Video pixel resolution.
         */
        Vector2i GetResolution() const;

        /** @brief Gets the active video's aspect ratio.
         *
         * @return Video aspect ratio.
         */
        float GetAspectRatio() const;

        /** @brief Gets the active video's current time in seconds.
         *
         * @return Current video time in seconds.
         */
        float GetTime() const;

        /** @brief Gets the active video's duration in seconds.
         *
         * @return Video duration in seconds.
         */
        float GetDuration() const;

        /** @brief Gets the active video's name.
         *
         * @return Active video name.
         */
        const std::string& GetName() const;

        /** @brief Gets the RGBA video frame image for the current time in the active video.
         *
         * @return RGBA video frame image.
         */
        const std::vector<byte>& GetVideoFrame() const;

        /** @brief Gets the L/R interlaced audio frame samples for the current time in the active video.
         *
         * @return Audio frame samples.
         */
        std::vector<float> GetAudioFrame();

        // ==========
        // Inquirers
        // ==========

        /** @brief Checks if an active video is loaded.
         *
         * @param `true` if an active video is loaded, `false` otherwise.
         */
        bool IsLoaded() const;

        /** @brief Checks if an active video is playing and hasn't ended.
         *
         * @return `true` if an active video is playing, `false` otherwise.
         */
        bool IsPlaying() const;

        // ==========
        // Utilities
        // ==========

        /** @brief Initializes the instance with a reference videos folder path.
         *
         * @param videosPath Folder path containing videos.
         */
        void Initialize(const std::filesystem::path& videosPath);

        /** @brief Starts a new active video.
         *
         * @param filename Filename of the video to play.
         * @return `true` if the video loaded successfully, `false` otherwise.
         */
        void Play(const std::string& filename);

        /** @brief Stops the active video and frees resources. */
        void Stop();

        /** @brief Progresses the active video.
         *
         * @param deltaTime Progression time in seconds.
         */
        void Update(float deltaTime);

        /** @brief Appends interleaved audio samples to the internal playback buffer.
         * Called by the `OnAudioFrame` callback.
         *
         * @param samples Array of float samples (L/R interleaved).
         * @param count Number of individual float values to append (`frames * channels`).
         */
        void AppendAudio(const float* samples, int count);
    };
}
