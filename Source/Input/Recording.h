#pragma once

#include "Input/Action.h"
#include "Input/AnalogAxis.h"

namespace Silent::Input
{
    /** @brief Input recorder state. */
    enum class RecorderState
    {
        None,
        Playing,
        Recording,
        StopPlaying,
        StopRecording
    };

    /** @brief Recorded input action event. */
    struct RecordedAction
    {
        ActionId ActionId = ActionId::Up;
        float    State    = 0.0f;
    };

    /** @brief Recorded input action event. */
    struct RecordedAnalogAxis
    {
        AnalogAxisId AnalogAxisId = AnalogAxisId::Move;
        Vector2      State        = Vector2::Zero;
    };

    /** @brief Recorded input event keyframe. */
    struct RecordedKeyframe
    {
        int                             FrameIdx   = 0;
        std::vector<RecordedAction>     Actions    = {};
        std::vector<RecordedAnalogAxis> AnalogAxes = {};
    };

    /** @brief Input recording. */
    struct Recording
    {
        int                           FrameCount = NO_VALUE;
        std::vector<RecordedKeyframe> Keyframes  = {};
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

        Recording                 _recording          = {};
        std::vector<ActionId>     _validActionIds     = {};
        std::vector<AnalogAxisId> _validAnalogAxisIds = {};

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

        /** @brief Gets the current recording.
         *
         * @note Used to retrieve a recording after stopping.
         *
         * @return Input recording.
         */
        const Recording& GetRecording() const;

        // ==========
        // Utilities
        // ==========

        void Update(std::vector<Action>& actions, std::vector<AnalogAxis>& axes);

        /** @brief Starts a new input recording playback.
         *
         * @note Only relevant actions and analog axes, defined by `validActionIds` and `validAxisIds`, will be played
         * back. Additionally, they must be locked before calling this method to commence playback.
         *
         * @param rec Input recording to play.
         * @param validActionIds Playable action IDs.
         * @param validAxisIds Playable analog axis IDs.
         */
        void Play(const Recording& rec,
                  const std::vector<ActionId>& validActionIds, const std::vector<AnalogAxisId>& validAxisIds);

        /** @brief Starts a new input recording.
         *
         * @note Only relevant actions and analog axes, defined by `validActionIds` and `validAxisIds`, are recorded.
         *
         * @param validActionIds Recordable action IDs.
         * @param validAxisIds Recordable analog axis IDs.
         */
        void Record(const std::vector<ActionId>& validActionIds, const std::vector<AnalogAxisId>& validAxisIds);

        /** @brief Signals the recorder to stop playing or recording on the next tick. */
        void Stop();

    private:
        // ========
        // Helpers
        // ========

        /** @brief Gets an active action event for the current frame.
         *
         * @return Active action event.
         */
        RecordedAction* GetActiveActionEvent(ActionId actionId);

        /** @brief Gets an active analog axis event for the current frame.
         *
         * @return Active analog axis event.
         */
        RecordedAnalogAxis* GetActiveAnalogAxisEvent(AnalogAxisId axisId);

        /** @brief Initializes the active frame with active actions and analog axes. */
        void InitializeActiveFrame();

        /** @brief Clears locked actions and analog axes.
         *
         * @param actions Input actions.
         * @param axes Input analog axes.
         */
        void ClearActionsAndAnalogAxes(std::vector<Action>& actions, std::vector<AnalogAxis>& axes);
    };
}
