#include "Framework.h"
#include "Assets/Parsers/Anm.h"

#include "Application.h"
#include "Utils/Stream.h"
#include "Utils/Utils.h"

using namespace Silent::Utils;

namespace Silent::Assets
{
    std::shared_ptr<void> ParseAnm(const std::filesystem::path& filename)
    {
        constexpr int ROT_MAT_COMP_COUNT = 9;

        const auto& fs = g_App.GetFilesystem();

        // Read file.
        auto stream = Stream(filename, true, false);
        if (!stream.IsOpen())
        {
            throw std::runtime_error(Fmt("Failed to open ANM `{}`.",
                                         std::filesystem::relative(filename, fs.GetAssetsDirectory()).string()));
        }

        // Read header.
        int16  keyframesOffset  = stream.ReadInt16();
        uint8  rotCount         = stream.ReadUint8();
        uint8  translationCount = stream.ReadUint8();
        int16  keyframeSize     = stream.ReadInt16();
        int16  boneCount        = stream.ReadInt16();
        int32  flags            = stream.ReadInt32();
        int32  endOffset        = stream.ReadInt32();
        uint16 keyframeCount    = stream.ReadUint16();
        uint8  scaleShift       = stream.ReadUint8(); // Unused.
        stream.Skip(1);

        int translationsSize = translationCount * Vector3i::AXIS_COUNT;
        int rotsSize         = rotCount * ROT_MAT_COMP_COUNT;
        Debug::Assert((translationsSize + rotsSize) == keyframeSize,
                      Fmt("Attempted to parse ANM `{}` with incongruent number of translations and rotations.",
                          std::filesystem::relative(filename, fs.GetAssetsDirectory()).string()));

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
                .BindTranslation = Vector3(bindTranslationX << scaleShift,
                                           bindTranslationY << scaleShift,
                                           bindTranslationZ << scaleShift)
            });
        }

        // Read keyframes.
        stream.SetPosition(keyframesOffset);
        asset.Keyframes.reserve(keyframeCount);
        for (int i = 0; i < keyframeCount; i++)
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
                int8 rotMat[3][3];
                stream.ReadArray(ToSpan(rotMat, ROT_MAT_COMP_COUNT));

                // Collect normalized bone rotation matrix.
                keyframe.BoneRotationMats.push_back(Matrix(rotMat[0][0] / 128.0f,
                                                           rotMat[1][0] / 128.0f,
                                                           rotMat[2][0] / 128.0f,
                                                           rotMat[0][1] / 128.0f,
                                                           rotMat[1][1] / 128.0f,
                                                           rotMat[2][1] / 128.0f,
                                                           rotMat[0][2] / 128.0f,
                                                           rotMat[1][2] / 128.0f,
                                                           rotMat[2][2] / 128.0f));
            }

            // Collect keyframe.
            asset.Keyframes.push_back(std::move(keyframe));
        }

        return std::make_shared<AnmAsset>(std::move(asset));
    }
}
