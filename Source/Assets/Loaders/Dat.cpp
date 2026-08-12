#include "Framework.h"
#include "Assets/Loaders/Dat.h"

#include "Application.h"
#include "Input/Input.h"
#include "Services/Filesystem.h"
#include "Utils/Stream.h"

using namespace Silent::Input;
using namespace Silent::Services;
using namespace Silent::Utils;

namespace Silent::Assets
{
    /** @brief DAT input recording controller key bindings for input actions. */
    struct DatControllerConfig
    {
        int Enter     = 0;
        int Cancel    = 0; // Unused.
        int Skip      = 0; // Unused.
        int Action    = 0;
        int Aim       = 0;
        int Light     = 0;
        int Run       = 0;
        int View      = 0;
        int StepLeft  = 0;
        int StepRight = 0;
        int Pause     = 0;
        int Item      = 0;
        int Map       = 0;
        int Option    = 0;
    };

    /** @brief DAT input recording PSX controller state. */
    struct DatControllerState
    {
        int ButtonFlags = 0;
        int RightX      = 0;
        int RightY      = 0;
        int LeftX       = 0;
        int LeftY       = 0;
    };

    /** @brief DAT input recording frame. */
    struct DatFrame
    {
        DatControllerState ControllerState   = {};
        int                ExpectedGameState = 0;
        uint               RandomSeed        = 0;
    };

    const void SetRecordedAction(RecordedKeyframe& recKeyframe, RecordedKeyframe& activeFrame,
                                 ActionId actionId, float state)
    {
        auto& activeEvent = activeFrame.Actions[(int)actionId];

        // Record action event if state changed.
        if (activeEvent.State != state)
        {
            recKeyframe.Actions.push_back(RecordedAction
            {
                .ActionId = actionId,
                .State    = state
            });

            // Update active event.
            activeEvent.State = state;
        }
    }

    const void SetRecordedAnalogAxis(RecordedKeyframe& recKeyframe, RecordedKeyframe& activeFrame,
                                     AnalogAxisId axisId, const Vector2& state)
    {
        auto& activeEvent = activeFrame.AnalogAxes[(int)axisId];

        // Record analog axis event if state changed.
        if (activeEvent.State != state)
        {
            recKeyframe.AnalogAxes.push_back(RecordedAnalogAxis
            {
                .AnalogAxisId = axisId,
                .State        = state
            });

            // Update active event.
            activeEvent.State = state;
        }
    }

    std::shared_ptr<void> ParseDat(const stdfs::path& filename)
    {
        const auto& fs = g_App.GetFilesystem();

        auto stateFilename = filename;
        auto playFilename  = filename;

        auto name = playFilename.filename().string();
        if (name.starts_with("DEMO"))
        {
            name.replace(0, 4, "PLAY");
            playFilename.replace_filename(name);
        }
        else
        {
            throw std::runtime_error(Fmt("Failed to open DAT `{}`. The file's name must start with `DEMO`, "
                                         "which contains the header. "
                                         "The associated `PLAY` file, containing playback frames, "
                                         "is read automatically.",
                                         stdfs::relative(filename, fs.GetAssetsDirectory()).string()));
        }

        // Read state file.
        auto stateStream = Stream(stateFilename, true, false);
        if (!stateStream.IsOpen())
        {
            throw std::runtime_error(Fmt("Failed to open DAT `{}`.",
                                         stdfs::relative(stateFilename, fs.GetAssetsDirectory()).string()));
        }

        // Read playback file.
        auto playStream = Stream(playFilename, true, false);
        if (!playStream.IsOpen())
        {
            throw std::runtime_error(Fmt("Failed to open DAT `{}`.",
                                         stdfs::relative(playFilename, fs.GetAssetsDirectory()).string()));
        }

        // Read controller config.
        auto contConfig = DatControllerConfig
        {
            .Enter     = stateStream.ReadUint16(),
            .Cancel    = stateStream.ReadUint16(),
            .Skip      = stateStream.ReadUint16(),
            .Action    = stateStream.ReadUint16(),
            .Aim       = stateStream.ReadUint16(),
            .Light     = stateStream.ReadUint16(),
            .Run       = stateStream.ReadUint16(),
            .View      = stateStream.ReadUint16(),
            .StepLeft  = stateStream.ReadUint16(),
            .StepRight = stateStream.ReadUint16(),
            .Pause     = stateStream.ReadUint16(),
            .Item      = stateStream.ReadUint16(),
            .Map       = stateStream.ReadUint16(),
            .Option    = stateStream.ReadUint16()
        };

        // Read options config.
        int8  screenPositionX  = stateStream.ReadInt8(); // Unused.
        int8  screenPositionY  = stateStream.ReadInt8(); // Unused.
        uint8 soundType        = stateStream.ReadUint8();
        uint8 volumeBgm        = stateStream.ReadUint8();
        uint8 volumeSe         = stateStream.ReadUint8();
        uint8 vibrationEnabled = stateStream.ReadUint8();
        uint8 brightness       = stateStream.ReadUint8();
        uint8 extraWeaponCtrl  = stateStream.ReadUint8();
        uint8 extraBloodColor  = stateStream.ReadUint8();
        int8  enableAutoLoad   = stateStream.ReadInt8();

        stateStream.Skip(1); // Unused.

        uint8 extraOptionsEnabled = stateStream.ReadUint8(); // Unused.
        int8  extraViewCtrl       = stateStream.ReadInt8();
        int8  extraViewMode       = stateStream.ReadInt8();
        int8  extraRetreatTurn    = stateStream.ReadInt8();
        int8  extraWalkRunCtrl    = stateStream.ReadInt8();
        int8  extraAutoAiming     = stateStream.ReadInt8();
        int8  extraBulletAdjust   = stateStream.ReadInt8();

        stateStream.Skip(2); // Unused.
        stateStream.Skip(4); // Unknown.

        uint32 palLanguageId = stateStream.ReadUint32();

        // Collect options config.
        auto optionsConfig = DatOptionsConfig
        {
            .SoundType         = soundType,
            .VolumeBgm         = volumeBgm,
            .VolumeSe          = volumeSe,
            .VibrationEnabled  = vibrationEnabled,
            .Brightness        = brightness,
            .ExtraWeaponCtrl   = extraWeaponCtrl,
            .ExtraBloodColor   = extraBloodColor,
            .EnableAutoLoad    = enableAutoLoad,
            .ExtraViewCtrl     = extraViewCtrl,
            .ExtraViewMode     = extraViewMode,
            .ExtraRetreatTurn  = extraRetreatTurn,
            .ExtraWalkRunCtrl  = extraWalkRunCtrl,
            .ExtraAutoAiming   = extraAutoAiming,
            .ExtraBulletAdjust = extraBulletAdjust,
            .PalLanguageId     = palLanguageId
        };

        stateStream.Skip(200);

        // @todo Read savegame.
        stateStream.Skip(636);

        // Collect savegame.
        auto savegame = DatSavegame{};

        stateStream.Skip(1148);

        uint32 frameCount = stateStream.ReadUint32();
        uint16 randSeed   = stateStream.ReadUint16();

        // Read recorded input frames.
        auto frames = std::vector<DatFrame>{};
        frames.reserve(frameCount);
        for (int i = 0; i < frameCount; i++)
        {
            // Read controller state.
            uint8  contStatus      = playStream.ReadUint8();  // Unused.
            uint8  contFlags       = playStream.ReadUint8();  // Unused.
            uint16 contButtonFlags = playStream.ReadUint16(); // Unused.
            uint8  contRightX      = playStream.ReadInt8(); // } @todo Check. Originally `uint8`, not sure if this breaks it.
            int8   contRightY      = playStream.ReadInt8(); // }
            int8   contLeftX       = playStream.ReadInt8(); // }
            int8   contLeftY       = playStream.ReadInt8(); // }

            int8 expectedGameState    = playStream.ReadInt8();
            int8 videoPresentInterval = playStream.ReadInt8(); // Unused. @todo Check if this can be ignored.

            playStream.Skip(2); // Unknown.

            uint32 randSeed = playStream.ReadUint32();

            // Collect recorded input frame.
            frames.push_back(DatFrame
            {
                .ControllerState = DatControllerState
                {
                    .ButtonFlags = contButtonFlags,
                    .RightX      = contRightX,
                    .RightY      = contRightY,
                    .LeftX       = contLeftX,
                    .LeftY       = contLeftY
                },
                .ExpectedGameState = expectedGameState
            });
        }

        auto seeds = std::vector<uint>{};
        seeds.reserve(frameCount);

        auto expectedGameStates = std::vector<int>{};
        expectedGameStates.reserve(frameCount);

        // Convert input recording.
        auto activeFrame  = RecordedKeyframe{};
        auto recKeyframes = std::vector<RecordedKeyframe>{};
        for (int i = 0; i < frames.size(); i++)
        {
            const auto& frame     = frames[i];
            const auto& contState = frame.ControllerState;

            activeFrame.FrameIdx = i;

            // Create recorded keyframe.
            auto recKeyframe = RecordedKeyframe{};
            SetRecordedAction(recKeyframe, activeFrame, ActionId::Up,        (contState.LeftY < 0) ? (abs(contState.LeftY) / 128.0f) : 0.0f);
            SetRecordedAction(recKeyframe, activeFrame, ActionId::Down,      (contState.LeftY > 0) ? (abs(contState.LeftY) / 128.0f) : 0.0f);
            SetRecordedAction(recKeyframe, activeFrame, ActionId::Left,      (contState.LeftX < 0) ? (abs(contState.LeftX) / 128.0f) : 0.0f);
            SetRecordedAction(recKeyframe, activeFrame, ActionId::Right,     (contState.LeftX > 0) ? (abs(contState.LeftX) / 128.0f) : 0.0f);
            SetRecordedAction(recKeyframe, activeFrame, ActionId::Enter,     (contState.ButtonFlags & contConfig.Enter)     ? 1.0f : 0.0f);
            SetRecordedAction(recKeyframe, activeFrame, ActionId::Action,    (contState.ButtonFlags & contConfig.Action)    ? 1.0f : 0.0f);
            SetRecordedAction(recKeyframe, activeFrame, ActionId::Aim,       (contState.ButtonFlags & contConfig.Aim)       ? 1.0f : 0.0f);
            SetRecordedAction(recKeyframe, activeFrame, ActionId::Light,     (contState.ButtonFlags & contConfig.Light)     ? 1.0f : 0.0f);
            SetRecordedAction(recKeyframe, activeFrame, ActionId::Run,       (contState.ButtonFlags & contConfig.Run)       ? 1.0f : 0.0f);
            SetRecordedAction(recKeyframe, activeFrame, ActionId::View,      (contState.ButtonFlags & contConfig.View)      ? 1.0f : 0.0f);
            SetRecordedAction(recKeyframe, activeFrame, ActionId::StepLeft,  (contState.ButtonFlags & contConfig.StepLeft)  ? 1.0f : 0.0f);
            SetRecordedAction(recKeyframe, activeFrame, ActionId::StepRight, (contState.ButtonFlags & contConfig.StepRight) ? 1.0f : 0.0f);
            SetRecordedAction(recKeyframe, activeFrame, ActionId::Pause,     (contState.ButtonFlags & contConfig.Pause)     ? 1.0f : 0.0f);
            SetRecordedAction(recKeyframe, activeFrame, ActionId::Item,      (contState.ButtonFlags & contConfig.Item)      ? 1.0f : 0.0f);
            SetRecordedAction(recKeyframe, activeFrame, ActionId::Map,       (contState.ButtonFlags & contConfig.Map)       ? 1.0f : 0.0f);
            SetRecordedAction(recKeyframe, activeFrame, ActionId::Option,    (contState.ButtonFlags & contConfig.Option)    ? 1.0f : 0.0f);
            SetRecordedAnalogAxis(recKeyframe, activeFrame, AnalogAxisId::Move,
                                  Vector2(contState.LeftX, contState.LeftY) / 128.0f);

            // Collect recorded keyframe.
            if (!recKeyframe.Actions.empty() || !recKeyframe.AnalogAxes.empty())
            {
                recKeyframe.FrameIdx = i;
                recKeyframes.push_back(std::move(recKeyframe));
            }

            // Collect seed and expected game state.
            seeds.push_back(frame.RandomSeed);
            expectedGameStates.push_back(frame.ExpectedGameState);
        }

        return std::make_shared<DatAsset>(DatAsset
        {
            .Options        = optionsConfig,
            .Savegame       = savegame,
            .InputRecording = Recording
            {
                .FrameCount = (int)frameCount,
                .Keyframes  = std::move(recKeyframes)
            },
            .BaseSeed = randSeed,
            .Seeds    = std::move(seeds)
        });
    }
}
