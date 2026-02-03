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
        int16  FramesOffset     = 0;
        uint8  RotationCount    = 0;
        uint8  TranslationCount = 0;
        int16  FrameSize        = 0;
        int16  BoneCount        = 0;
        int32  Flags            = 0;
        int32  EndOffset        = 0;
        uint16 FrameCount       = 0;
        uint8  ScaleShift       = 0;
    };

    /** @brief ANM translation layout. */
    struct AnmTranslationLayout
    {
        int8 X = 0;
        int8 Y = 0;
        int8 Z = 0;
    };

    /** @brief ANM 3x3 rotation matrix layout. */
    struct AnmRotationLayout
    {
        static constexpr int MAT_COMP_COUNT = 9;

        std::array<int8, MAT_COMP_COUNT> RotationMat = {};
    };

    /** @brief ANM bone layout. */
    struct AnmBoneLayout
    {
        int8                 ParentIdx       = 0;
        int8                 RotationIdx     = 0;
        int8                 TranslationIdx  = 0;
        AnmTranslationLayout BindTranslation = {};
    };

    /** @brief ANM keyframe layout. */
    struct AnmKeyframeLayout
    {
        std::vector<AnmTranslationLayout> Translations = {};
        std::vector<AnmRotationLayout>    Rotations    = {};
    };

    std::shared_ptr<void> ParseAnm(const std::filesystem::path& filename)
    {
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
        int rotsSize         = header.RotationCount * AnmRotationLayout::MAT_COMP_COUNT;
        Debug::Assert((translationsSize + rotsSize) == header.FrameSize,
                      "Attempted to parse ANM with incongruant translations and rotations size.");

        // Read bones.
        auto bones = std::vector<AnmBoneLayout>{};
        bones.reserve(header.BoneCount);
        for (int i = 0; i < header.BoneCount; i++)
        {
            bones.push_back(AnmBoneLayout
            {
                .ParentIdx       = stream.ReadInt8(),
                .RotationIdx     = stream.ReadInt8(),
                .TranslationIdx  = stream.ReadInt8(),
                .BindTranslation = AnmTranslationLayout
                {
                    stream.ReadInt8(),
                    stream.ReadInt8(),
                    stream.ReadInt8()
                }
            });
        }

        // Read keyframes.
        stream.SetPosition(header.FramesOffset);
        auto keyframes = std::vector<AnmKeyframeLayout>{};
        keyframes.reserve(header.FrameCount);
        for (int i = 0; i < header.FrameCount; i++)
        {
            auto keyframe = AnmKeyframeLayout{};

            // Read bone translations.
            keyframe.Translations.reserve(header.TranslationCount);
            for (int j = 0; j < header.TranslationCount; j++)
            {
                keyframe.Translations.push_back(AnmTranslationLayout
                {
                    stream.ReadInt8(),
                    stream.ReadInt8(),
                    stream.ReadInt8()
                });
            }

            // Read bone rotations.
            keyframe.Rotations.reserve(header.RotationCount);
            for (int j = 0; j < header.RotationCount; j++)
            {
                keyframe.Rotations.emplace_back();
                stream.ReadArray(ToSpan(keyframe.Rotations.back().RotationMat));
            }

            keyframes.push_back(keyframe);
        }

        // Create asset.
        auto asset = AnmAsset
        {
            // @todo
        };

        return std::make_shared<AnmAsset>(std::move(asset));
    }
}
