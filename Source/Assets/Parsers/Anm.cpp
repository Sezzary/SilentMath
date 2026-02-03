#include "Framework.h"
#include "Assets/Parsers/Anm.h"

#include "Utils/Stream.h"
#include "Utils/Utils.h"

using namespace Silent::Utils;

namespace Silent::Assets
{
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
        int16 framesOffset     = stream.ReadInt16();
        uint8 rotCount         = stream.ReadUint8();
        uint8 translationCount = stream.ReadUint8();
        int16 frameSize        = stream.ReadInt16();
        int16 boneCount        = stream.ReadInt16();
        int32 flags            = stream.ReadInt32();
        int32 endOffset        = stream.ReadInt32(); // Unused.
        int16 frameCount       = stream.ReadUint16();
        int8  scaleShift       = stream.ReadUint8();
        stream.ReadUint8(); // Padding.

        int translationsSize = translationCount * Vector3i::AXIS_COUNT;
        int rotsSize         = rotCount * ROT_MAT_COMP_COUNT;
        Debug::Assert((translationsSize + rotsSize) == frameSize,
                      "Attempted to parse ANM with incongruent number of translations and rotations.");

        // Create asset.
        auto asset = AnmAsset
        {
            .Flags = flags
        };

        // Read bones.
        asset.Bones.reserve(boneCount);
        for (int i = 0; i < boneCount; i++)
        {
            int8 parentBoneIdx    = stream.ReadInt8();
            int8 rotIdx           = stream.ReadInt8();
            int8 TranslationIdx   = stream.ReadInt8();
            int8 BindTranslationX = stream.ReadInt8();
            int8 BindTranslationY = stream.ReadInt8();
            int8 BindTranslationZ = stream.ReadInt8();

            // Collect bone.
            asset.Bones.push_back(AnmBone
            {
                .ParentBoneIdx   = parentBoneIdx,
                .RotationIdx     = rotIdx,
                .TranslationIdx  = TranslationIdx,
                .BindTranslation = Vector3(BindTranslationX, BindTranslationY, BindTranslationZ)
            });
        }

        // Read keyframes.
        stream.SetPosition(framesOffset);
        asset.Keyframes.reserve(frameCount);
        for (int i = 0; i < frameCount; i++)
        {
            auto keyframe = AnmKeyframe{};

            // Read bone translations.
            keyframe.BoneTranslations.reserve(translationCount);
            for (int j = 0; j < translationCount; j++)
            {
                int8 translationX = stream.ReadInt8();
                int8 translationY = stream.ReadInt8();
                int8 translationZ = stream.ReadInt8();

                // Collect bone translation.
                keyframe.BoneTranslations.push_back(Vector3(translationX << scaleShift,
                                                            translationY << scaleShift,
                                                            translationZ << scaleShift));
            }

            // Read bone rotations.
            keyframe.BoneRotationMats.reserve(rotCount);
            for (int j = 0; j < rotCount; j++)
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
