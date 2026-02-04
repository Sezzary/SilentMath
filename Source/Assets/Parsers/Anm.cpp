#include "Framework.h"
#include "Assets/Parsers/Anm.h"

#include "Utils/Stream.h"
#include "Utils/Utils.h"

using namespace Silent::Utils;

namespace Silent::Assets
{
    /** @brief ANM header layout. */
    struct AnmHeaderLayout
    {
        int16 FramesOffset     = 0;
        uint8 RotationCount    = 0;
        uint8 TranslationCount = 0;
        int16 FrameSize        = 0;
        int16 BoneCount        = 0;
        int32 Flags            = 0;
        int32 EndOffset        = 0;
        int16 FrameCount       = 0;
        int8  ScaleShift       = 0;  /** Unused. */
    };

    std::shared_ptr<void> ParseAnm(const std::filesystem::path& filename)
    {
        constexpr int ROT_MAT_COMP_COUNT = 9;

        // Read file.
        auto stream = Stream(filename, true, false);
        if (!stream.IsOpen())
        {
            throw std::runtime_error(Fmt("Failed to open ANM `{}`.", filename.string()));
        }

        // Read header.
        auto header = AnmHeaderLayout
        {
            .FramesOffset     = stream.ReadInt16(),
            .RotationCount    = stream.ReadUint8(),
            .TranslationCount = stream.ReadUint8(),
            .FrameSize        = stream.ReadInt16(),
            .BoneCount        = stream.ReadInt16(),
            .Flags            = stream.ReadInt32(),
            .EndOffset        = stream.ReadInt32(),
            .FrameCount       = stream.ReadUint16(),
            .ScaleShift       = stream.ReadUint8()
        };
        stream.ReadUint8(); // Padding.

        int translationsSize = header.TranslationCount * Vector3i::AXIS_COUNT;
        int rotsSize         = header.RotationCount * ROT_MAT_COMP_COUNT;
        Debug::Assert((translationsSize + rotsSize) == header.FrameSize,
                      "Attempted to parse ANM with incongruent number of translations and rotations.");

        // Create asset.
        auto asset = AnmAsset
        {
            .Flags = header.Flags
        };

        // Read bones.
        asset.Bones.reserve(header.BoneCount);
        for (int i = 0; i < header.BoneCount; i++)
        {
            int8 parentBoneIdx    = stream.ReadInt8();
            int8 rotIdx           = stream.ReadInt8();
            int8 translationIdx   = stream.ReadInt8();
            int8 bindTranslationX = stream.ReadInt8();
            int8 bindTranslationY = stream.ReadInt8();
            int8 bindTranslationZ = stream.ReadInt8();

            // Collect bone.
            asset.Bones.push_back(AnmBone
            {
                .ParentBoneIdx   = parentBoneIdx,
                .TranslationIdx  = translationIdx,
                .RotationIdx     = rotIdx,
                .BindTranslation = Vector3(bindTranslationX << header.ScaleShift,
                                           bindTranslationY << header.ScaleShift,
                                           bindTranslationZ << header.ScaleShift)
            });
        }

        // Read keyframes.
        stream.SetPosition(header.FramesOffset);
        asset.Keyframes.reserve(header.FrameCount);
        for (int i = 0; i < header.FrameCount; i++)
        {
            auto keyframe = AnmKeyframe{};

            // Read bone translations.
            keyframe.BoneTranslations.reserve(header.TranslationCount);
            for (int j = 0; j < header.TranslationCount; j++)
            {
                int8 translationX = stream.ReadInt8();
                int8 translationY = stream.ReadInt8();
                int8 translationZ = stream.ReadInt8();

                // Collect bone translation.
                keyframe.BoneTranslations.push_back(Vector3(translationX << header.ScaleShift,
                                                            translationY << header.ScaleShift,
                                                            translationZ << header.ScaleShift));
            }

            // Read bone rotations.
            keyframe.BoneRotationMats.reserve(header.RotationCount);
            for (int j = 0; j < header.RotationCount; j++)
            {
                auto rotMat = std::array<int8, ROT_MAT_COMP_COUNT>{};
                stream.ReadArray(ToSpan(rotMat));

                // Collect bone rotation matrix. @todo Convert to non-PSX matrix.
                keyframe.BoneRotationMats.emplace_back();
            }

            // Collect keyframe.
            asset.Keyframes.push_back(std::move(keyframe));
        }

        return std::make_shared<AnmAsset>(std::move(asset));
    }
}
