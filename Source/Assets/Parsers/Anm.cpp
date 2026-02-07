#include "Framework.h"
#include "Assets/Parsers/Anm.h"

#include "Application.h"
#include "Utils/Stream.h"
#include "Utils/Utils.h"

using namespace Silent::Utils;

namespace Silent::Assets
{
    /** @brief ANM header layout. */
    struct AnmHeaderLayout
    {
        int16 KeyframesOffset  = 0;
        uint8 RotationCount    = 0;
        uint8 TranslationCount = 0;
        int16 KeyframeSize     = 0;
        int16 BoneCount        = 0;
        int32 Flags            = 0;
        int32 EndOffset        = 0;
        int16 KeyframeCount    = 0;
        int8  ScaleShift       = 0;  /** Unused. */
    };

    std::shared_ptr<void> ParseAnm(const std::filesystem::path& filename)
    {
        constexpr int ROT_MAT_COMP_COUNT = 9;

        const auto& fs = g_App.GetFilesystem();

        // Read file.
        auto stream = Stream(filename, true, false);
        if (!stream.IsOpen())
        {
            throw std::runtime_error(Fmt("Failed to open ANM `{}`.", std::filesystem::relative(fs.GetAssetsDirectory(), filename).string()));
        }

        // Read header.
        auto header = AnmHeaderLayout
        {
            .KeyframesOffset  = stream.ReadInt16(),
            .RotationCount    = stream.ReadUint8(),
            .TranslationCount = stream.ReadUint8(),
            .KeyframeSize     = stream.ReadInt16(),
            .BoneCount        = stream.ReadInt16(),
            .Flags            = stream.ReadInt32(),
            .EndOffset        = stream.ReadInt32(),
            .KeyframeCount    = stream.ReadUint16(),
            .ScaleShift       = stream.ReadUint8()
        };
        stream.Skip(1);

        int translationsSize = header.TranslationCount * Vector3i::AXIS_COUNT;
        int rotsSize         = header.RotationCount * ROT_MAT_COMP_COUNT;
        Debug::Assert((translationsSize + rotsSize) == header.KeyframeSize,
                      Fmt("Attempted to parse ANM `{}` with incongruent number of translations and rotations.",
                          std::filesystem::relative(fs.GetAssetsDirectory(), filename).string()));

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
        stream.SetPosition(header.KeyframesOffset);
        asset.Keyframes.reserve(header.KeyframeCount);
        for (int i = 0; i < header.KeyframeCount; i++)
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
                int8 rotMat[3][3];
                stream.ReadArray(ToSpan(rotMat, ROT_MAT_COMP_COUNT));

                // Collect normalized bone rotation matrix.
                keyframe.BoneRotationMats.push_back(Matrix(rotMat[0][0] / 128.0f, rotMat[1][0] / 128.0f, rotMat[2][0] / 128.0f,
                                                           rotMat[0][1] / 128.0f, rotMat[1][1] / 128.0f, rotMat[2][1] / 128.0f,
                                                           rotMat[0][2] / 128.0f, rotMat[1][2] / 128.0f, rotMat[2][2] / 128.0f));
            }

            // Collect keyframe.
            asset.Keyframes.push_back(std::move(keyframe));
        }

        return std::make_shared<AnmAsset>(std::move(asset));
    }
}
