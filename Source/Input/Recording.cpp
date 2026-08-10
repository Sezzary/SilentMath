#include "Framework.h"
#include "Input/Recording.h"

#include "Input/Action.h"
#include "Input/AnalogAxis.h"
#include "Utils/Utils.h"

using namespace Silent::Utils;

namespace Silent::Input
{
    RecorderState Recorder::GetState() const
    {
        return _state;
    }

    const Recording& Recorder::GetRecording() const
    {
        return _recording;
    }

    void Recorder::Update(std::vector<Action>& actions, std::vector<AnalogAxis>& axes)
    {
        switch (_state)
        {
            default:
            case RecorderState::None:
            {
                return;
            }
            case RecorderState::Playing:
            {
                // Check if playback is finished.
                if (_frameIdx >= _recording.FrameCount)
                {
                    _state = RecorderState::StopPlaying;
                    return;
                }

                // Clear actions and analog axes on initial frame.
                if (_frameIdx == 0)
                {
                    ClearActionsAndAnalogAxes(actions, axes);
                }

                // Update active frame.
                if (_keyframeIdx < _recording.Keyframes.size())
                {
                    const auto& nextKeyframe = _recording.Keyframes[_keyframeIdx];
                    if (nextKeyframe.FrameIdx == _frameIdx)
                    {
                        for (const auto& [keyActionId, event] : nextKeyframe.Actions)
                        {
                            auto& activeEvent = _activeFrame.Actions[keyActionId];
                            activeEvent       = event;
                        }
    
                        for (const auto& [keyAxisId, event] : nextKeyframe.AnalogAxes)
                        {
                            auto& activeEvent = _activeFrame.AnalogAxes[keyAxisId];
                            activeEvent       = event;
                        }
    
                        _keyframeIdx++;
                    }
                }

                // Update locked action states.
                for (const auto& [keyActionId, state] : _activeFrame.Actions)
                {
                    auto& action = actions[(int)keyActionId];
                    if (action.IsLocked())
                    {
                        action.Update(state);
                    }
                }

                // Update locked analog axis states.
                for (const auto& [keyAxisId, state] : _activeFrame.AnalogAxes)
                {
                    auto& axis = axes[(int)keyAxisId];
                    if (axis.IsLocked())
                    {
                        axis.State = state;
                    }
                }
                break;
            }
            case RecorderState::Recording:
            {
                _activeFrame.FrameIdx = _frameIdx;
                auto recKeyframe      = RecordedKeyframe{};

                // Clear actions and analog axes on initial frame.
                if (_frameIdx == 0)
                {
                    ClearActionsAndAnalogAxes(actions, axes);
                }

                // Run through valid action IDs.
                for (const auto actionId : _validActionIds)
                {
                    const auto& action      = actions[(int)actionId];
                    float&      activeState = _activeFrame.Actions[action.GetId()];

                    // Record new action event on initial recorded keyframe.
                    if (_frameIdx == 0)
                    {
                        float& recState = recKeyframe.Actions[action.GetId()];
                        recState        = action.GetState();
                    }
                    // Record new action event if state changed.
                    else if (activeState != action.GetState())
                    {
                        float& recState = recKeyframe.Actions[action.GetId()];
                        recState        = action.GetState();
                    }

                    // Update active event.
                    activeState = action.GetState();
                }

                // Run through valid analog axis IDs.
                for (auto axisId : _validAnalogAxisIds)
                {
                    const auto& axis        = axes[(int)axisId];
                    auto&       activeState = _activeFrame.AnalogAxes[axis.GetId()];

                    // Record new action event on initial recorded keyframe.
                    if (_frameIdx == 0)
                    {
                        auto& recState = recKeyframe.AnalogAxes[axis.GetId()];
                        recState       = axis.State;
                    }
                    // Record new action event if state changed.
                    else if (activeState != axis.State)
                    {
                        auto& recState = recKeyframe.AnalogAxes[axis.GetId()];
                        recState       = axis.State;
                    }

                    // Update active event.
                    activeState = axis.State;
                }

                // Add new recorded keyframe.
                if (!recKeyframe.Actions.empty() || !recKeyframe.AnalogAxes.empty())
                {
                    recKeyframe.FrameIdx = _frameIdx;
                    _recording.Keyframes.push_back(std::move(recKeyframe));
                }
                break;
            }
            case RecorderState::StopPlaying:
            {
                _state = RecorderState::None;

                // Update action states.
                for (const auto& [keyActionId, event] : _activeFrame.Actions)
                {
                    auto& action = actions[(int)keyActionId];
                    if (action.IsLocked())
                    {
                        action.Clear();
                    }
                }

                // Update analog axis states.
                for (const auto& [keyAxisId, event] : _activeFrame.AnalogAxes)
                {
                    auto& axis = axes[(int)keyAxisId];
                    if (axis.IsLocked())
                    {
                        axis.State = Vector2::Zero;
                    }
                }

                Debug::Log("Stopped input recording playback.");
                return;
            }
            case RecorderState::StopRecording:
            {
                _state                = RecorderState::None;
                _recording.FrameCount = _frameIdx + 1;

                Debug::Log("Stopped input recording.");
                return;
            }
        }

        _frameIdx++;
    }

    void Recorder::Play(const Recording& rec,
                        const std::set<ActionId>& validActionIds, const std::set<AnalogAxisId>& validAxisIds)
    {
        // Check if recorder is idle.
        if (_state != RecorderState::None)
        {
            Debug::Log(Fmt("Attempted to interrupt input recorder with new playback while in state {}.", (int)_state),
                       Debug::LogLevel::Error);
            return;
        }

        // Check if recording is valid.
        if (rec.FrameCount == NO_VALUE)
        {
            Debug::Log(Fmt("Attempted to play invalid input recording.", (int)_state), Debug::LogLevel::Error);
            return;
        }

        // Setup.
        _state              = RecorderState::Playing;
        _frameIdx           = 0;
        _keyframeIdx        = 0;
        _recording          = rec;
        _validActionIds     = validActionIds;
        _validAnalogAxisIds = validAxisIds;
        InitializeActiveFrame();

        Debug::Log("Input recording playback started.");
    }

    void Recorder::Record(const std::set<ActionId>& validActionIds, const std::set<AnalogAxisId>& validAxisIds)
    {
        // Check if recorder is idle.
        if (_state != RecorderState::None)
        {
            Debug::Log(Fmt("Attempted to interrupt input recorder with new recording while in state {}.", (int)_state),
                       Debug::LogLevel::Error);
            return;
        }

        // Setup.
        _state              = RecorderState::Recording;
        _frameIdx           = 0;
        _keyframeIdx        = 0;
        _recording          = {};
        _validActionIds     = validActionIds;
        _validAnalogAxisIds = validAxisIds;
        InitializeActiveFrame();

        Debug::Log("Input recording started.");
    }

    void Recorder::Stop()
    {
        switch (_state)
        {
            case RecorderState::Playing:
            {
                _state = RecorderState::StopPlaying;
                break;
            }
            case RecorderState::Recording:
            {
                _state = RecorderState::StopRecording;
                break;
            }
            default:
            {
                Debug::Log(Fmt("Attempted to stop input recorder while in state {}.", (int)_state),
                           Debug::LogLevel::Error);
                break;
            }
        }
    }

    void Recorder::InitializeActiveFrame()
    {
        _activeFrame = {};

        // Set action event states.
        for (auto actionId : _validActionIds)
        {
            _activeFrame.Actions[actionId] = 0.0f;
        }

        // Set analog axis event states.
        for (auto axisId : _validAnalogAxisIds)
        {
            _activeFrame.AnalogAxes[axisId] = Vector2::Zero;
        }
    }

    void Recorder::ClearActionsAndAnalogAxes(std::vector<Action>& actions, std::vector<AnalogAxis>& axes)
    {
        // Clear locked actions.
        for (auto actionId : _validActionIds)
        {
            auto& action = actions[(int)actionId];
            if (action.IsLocked())
            {
                action.Clear();
            }
        }

        // Clear locked analog axes.
        for (auto axisId : _validAnalogAxisIds)
        {
            auto& axis = axes[(int)axisId];
            if (axis.IsLocked())
            {
                axis.State = Vector2::Zero;
            }
        }
    }
}
