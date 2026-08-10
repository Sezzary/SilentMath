#pragma once

#include "Input/Action.h"
#include "Input/AnalogAxis.h"

namespace Silent::Input
{
    /** @brief Input recorder state */
    enum class RecorderState
    {
        None,
        Playing,
        Recording,
        StopPlaying,
        StopRecording
    };

    /** @brief Recorded input event keyframe. */
    struct RecordedKeyframe
    {
        int                                       FrameIdx   = 0;
        std::unordered_map<ActionId, float>       Actions    = {}; /** Key = action ID, value = action state. */
        std::unordered_map<AnalogAxisId, Vector2> AnalogAxes = {}; /** Key = analog axis ID, value = analog axis state. */
    };

    /** @brief Input recording and playback manager. */
    class Recorder
    {
    private:
        // =======
        // Fields
        // =======

        RecorderState    _state       = RecorderState::None;
        RecordedKeyframe _activeFrame = {};
        int              _frameIdx    = 0;
        int              _keyframeIdx = 0;

        int                           _frameCount            = 0;
        std::vector<RecordedKeyframe> _keyframes             = {};
        std::set<ActionId>            _relevantActionIds     = {};
        std::set<AnalogAxisId>        _relevantAnalogAxisIds = {};

    public:
        // =============
        // Constructors
        // =============

        /** @brief Creates a default initialized instance. */
        Recorder() = default;

        // ========
        // Getters
        // ========

        /** @brief Gets the active recorder state.
         *
         * @return Recorder state.
         */
        RecorderState GetState() const;

        // ==========
        // Utilities
        // ==========

        void Update(std::vector<Action>& actions, std::vector<AnalogAxis>& axes);

        /** @brief
         *
         * @param frameCount Playback frame count.
         * @param recKeyframes Recorded input event keyframes.
         */
        void Play(int frameCount, const std::vector<RecordedKeyframe>& recKeyframes,
                  const std::set<ActionId>& relevantActionIds, const std::set<AnalogAxisId>& relevantAxisIds);

        void Record(std::vector<Action>& actions, std::vector<AnalogAxis>& axes,
                    const std::set<ActionId>& relevantActionIds, const std::set<AnalogAxisId>& relevantAxisIds);

        /** @brief Signals the recorder to stop playback and clear input states on the next tick. */
        const std::vector<RecordedKeyframe>& Stop();

    private:
        // ========
        // Helpers
        // ========

        void Reset();
    };
}
