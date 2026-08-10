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
                if (_frameIdx >= _frameCount)
                {
                    _state = RecorderState::StopPlaying;
                    return;
                }

                // Update active frame.
                const auto& nextKeyframe = _keyframes[_keyframeIdx];
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

                // Update action states.
                for (const auto& [keyActionId, state] : _activeFrame.Actions)
                {
                    auto& action = actions[(int)keyActionId];
                    if (action.IsLocked())
                    {
                        action.Update(state);
                    }
                }

                // Update analog axis states.
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

                // Run through relevant action IDs.
                for (const auto actionId : _relevantActionIds)
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

                // Run through relevant analog axis IDs.
                for (auto axisId : _relevantAnalogAxisIds)
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
                    _keyframes.push_back(std::move(recKeyframe));
                }
                break;
            }
            case RecorderState::StopPlaying:
            {
                // Update action states.
                for (const auto& [keyActionId, event] : _activeFrame.Actions)
                {
                    auto& action = actions[(int)keyActionId];
                    if (action.IsLocked())
                    {
                        action.Update(0.0f);
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
                // @todo Then fetch?

                Debug::Log("Stopped input recording.");
                return;
            }
        }

        _frameIdx++;
    }

    void Recorder::Play(int frameCount, const std::vector<RecordedKeyframe>& recKeyframes,
                        const std::set<ActionId>& relevantActionIds, const std::set<AnalogAxisId>& relevantAxisIds)
    {
        if (_state != RecorderState::None)
        {
            Debug::Log(Fmt("Attempted to interrupt input recorder with new playback while in state {}.", (int)_state),
                       Debug::LogLevel::Error);
            return;
        }
        Debug::Log("Input recording playback started.");

        frameCount = std::clamp(frameCount, 0, frameCount);

        // Setup.
        Reset();
        _state                 = RecorderState::Playing;
        _frameCount            = frameCount;
        _keyframes             = recKeyframes;
        _relevantActionIds     = relevantActionIds;
        _relevantAnalogAxisIds = relevantAxisIds;

        // Set action event states for initial active frame.
        for (auto actionId : relevantActionIds)
        {
            _activeFrame.Actions[actionId] = 0.0f;
        }

        // Set analog axis event states for initial active frame.
        for (auto axisId : relevantAxisIds)
        {
            _activeFrame.AnalogAxes[axisId] = Vector2::Zero;
        }
    }

    void Recorder::Record(std::vector<Action>& actions, std::vector<AnalogAxis>& axes,
                          const std::set<ActionId>& relevantActionIds, const std::set<AnalogAxisId>& relevantAxisIds)
    {
        if (_state != RecorderState::None)
        {
            Debug::Log(Fmt("Attempted to interrupt input recorder with new recording while in state {}.", (int)_state),
                       Debug::LogLevel::Error);
            return;
        }

        Debug::Log("Input recording started.");

        Reset();
        _state                 = RecorderState::Recording;
        _relevantActionIds     = relevantActionIds;
        _relevantAnalogAxisIds = relevantAxisIds;

        for (auto actionId : relevantActionIds)
        {
            const auto& action      = actions[(int)actionId];
            auto&       activeEvent = _activeFrame.Actions[action.GetId()];
        }
    }

    const std::vector<RecordedKeyframe>& Recorder::Stop()
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

    void Recorder::Reset()
    {
        _state       = RecorderState::None;
        _frameIdx    = 0;
        _keyframeIdx = 0;
        _activeFrame = {};
        _frameCount  = 0;
        _keyframes.clear();
        _relevantActionIds.clear();
        _relevantAnalogAxisIds.clear();
    }
}
