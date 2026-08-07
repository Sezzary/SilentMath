#include "Framework.h"
#include "Assets/Loaders/Dms.h"

#include "Application.h"
#include "Services/Filesystem.h"
#include "Utils/Stream.h"

using namespace Silent::Services;
using namespace Silent::Utils;

namespace Silent::Assets
{
    std::shared_ptr<void> ParseDms(const stdfs::path& filename)
    {
        const auto& fs = g_App.GetFilesystem();

        // Read file.
        auto stream = Stream(filename, true, false);
        if (!stream.IsOpen())
        {
            throw std::runtime_error(Fmt("Failed to open DMS `{}`.",
                                         stdfs::relative(filename, fs.GetAssetsDirectory()).string()));
        }

        // Read header.
        stream.Skip(1);

        uint8 charaEntryCount = stream.ReadUint8();
        uint8 segmentCount    = stream.ReadUint8();

        stream.Skip(5);

        uint32 segmentsOffset = stream.ReadUint32();

        q23_8 originX = stream.ReadInt32();
        q23_8 originY = stream.ReadInt32();
        q23_8 originZ = stream.ReadInt32();

        uint32 charaEntriesOffset = stream.ReadUint32();

        // Read camera entry.
        int16 camEntryKeyframeCount  = stream.ReadInt16();
        uint8 camEntryHoldRangeCount = stream.ReadUint8();

        stream.Skip(1);

        auto   camEntryName             = stream.ReadCString(4);
        uint32 camEntryHoldRangesOffset = stream.ReadUint32();
        uint32 camEntryKeyframesOffset  = stream.ReadUint32();

        // Set stream position to camera hold ranges.
        stream.SetPosition(camEntryHoldRangesOffset);

        // Read camera hold ranges.
        auto camHoldRanges = std::vector<DmsHoldRange>{};
        camHoldRanges.reserve(camEntryHoldRangeCount);
        for (int i = 0; i < camEntryHoldRangeCount; i++)
        {
            // Collect camera hold range.
            camHoldRanges.push_back(DmsHoldRange
            {
                .StartFrameIdx = stream.ReadInt16(),
                .EndFrameIdx   = stream.ReadInt16(),
                .KeyframeIdx   = stream.ReadInt16()
            });
        }

        // Set stream position to camera keyframes.
        stream.SetPosition(camEntryKeyframesOffset);

        // Read camera keyframes.
        auto camKeyframes = std::vector<DmsKeyframeType>{};
        camKeyframes.reserve(camEntryKeyframeCount);
        for (int i = 0; i < camEntryKeyframeCount; i++)
        {
            // Read position target.
            q7_8 posTargetX = stream.ReadInt16();
            q7_8 posTargetY = stream.ReadInt16();
            q7_8 posTargetZ = stream.ReadInt16();

            // Read look-at target.
            q7_8 lookAtTargetX = stream.ReadInt16();
            q7_8 lookAtTargetY = stream.ReadInt16();
            q7_8 lookAtTargetZ = stream.ReadInt16();

            stream.Skip(2);

            q7_8 projDist = stream.ReadInt16();

            // Collect camera keyframe.
            camKeyframes.push_back(DmsKeyframeCamera
            {
                .PositionTarget     = Vector3i(posTargetX, posTargetY, posTargetZ),
                .LookAtTarget       = Vector3i(lookAtTargetX, lookAtTargetY, lookAtTargetZ),
                .ProjectionDistance = projDist
            });
        }

        // Set stream position to segments.
        stream.SetPosition(segmentsOffset);

        // Read segments.
        auto segments = std::vector<DmsSegment>{};
        segments.reserve(segmentCount);
        for (int i = 0; i < segmentCount; i++)
        {
            // Collect segment.
            segments.push_back(DmsSegment
            {
                .StartFrameIdx = stream.ReadInt16(),
                .FrameCount    = stream.ReadInt16()
            });
        }

        // Set stream position to character entries.
        stream.SetPosition(charaEntriesOffset);

        // Read character entries.
        auto charaEntries = std::vector<DmsEntry>{};
        charaEntries.reserve(charaEntryCount);
        for (int i = 0; i < charaEntryCount; i++)
        {
            int16 keyframeCount  = stream.ReadInt16();
            uint8 holdRangeCount = stream.ReadInt8();

            stream.Skip(1);

            auto   name             = stream.ReadCString(4);
            uint32 holdRangesOffset = stream.ReadUint32();
            uint32 keyframesOffset  = stream.ReadUint32();

            // Store current stream position.
            int streamPos = stream.GetPosition();

            // Set stream position to character hold ranges.
            stream.SetPosition(holdRangesOffset);

            // Read character hold ranges.
            auto holdRanges = std::vector<DmsHoldRange>{};
            holdRanges.reserve(holdRangeCount);
            for (int j = 0; j < holdRangeCount; j++)
            {
                // Collect character hold range.
                holdRanges.push_back(DmsHoldRange
                {
                    .StartFrameIdx = stream.ReadInt16(),
                    .EndFrameIdx   = stream.ReadInt16(),
                    .KeyframeIdx   = stream.ReadInt16()
                });
            }

            // Set stream position to character keyframes.
            stream.SetPosition(holdRangesOffset);

            // Read character keyframes.
            auto charaKeyframes = std::vector<DmsKeyframeType>{};
            charaKeyframes.reserve(keyframeCount);
            for (int j = 0; j < keyframeCount; j++)
            {
                // Read position.
                q7_8 posX = stream.ReadInt16();
                q7_8 posY = stream.ReadInt16();
                q7_8 posZ = stream.ReadInt16();

                // Read rotation.
                q3_12 rotX = stream.ReadInt16();
                q3_12 rotY = stream.ReadInt16();
                q3_12 rotZ = stream.ReadInt16();

                // Collect character keyframe.
                charaKeyframes.push_back(DmsKeyframeCharacter
                {
                    .Position = Vector3i(posX, posY, posZ),
                    .Rotation = Vector3i(rotX, rotY, rotZ)
                });
            }

            // Collect character entry.
            charaEntries.push_back(DmsEntry
            {
                .Name       = name,
                .HoldRanges = std::move(holdRanges),
                .Keyframes  = std::move(charaKeyframes)
            });

            // Restore stream position.
            stream.SetPosition(streamPos);
        }

        return std::make_shared<DmsAsset>(DmsAsset
        {
            .Origin           = Vector3i(originX, originY, originZ),
            .Segments         = std::move(segments),
            .CharacterEntries = std::move(charaEntries),
            .CameraEntry      = DmsEntry
            {
                .Name       = camEntryName,
                .HoldRanges = std::move(camHoldRanges),
                .Keyframes  = std::move(camKeyframes)
            }
        });
    }
}
