#pragma once

namespace Silent::Assets
{
    // @todo A revised .ANM2 format should also pack the hardcoded anim infos into each file.

    /** @brief ANM bone. */
    struct AnmBone
    {
        int     ParentIdx       = 0;
        int     RotationIdx     = 0;
        int     TranslationIdx  = 0;
        Vector3 BindTranslation = Vector3::Zero;
    };

    /** @brief ANM keyframe. */
    struct AnmKeyframe
    {
        std::vector<Vector3> BoneTranslations = {};
        std::vector<Matrix>  BoneRotations    = {};
    };

    /** @brief ANM asset data. */
    struct AnmAsset
    {
        std::vector<AnmBone>     Bones     = {};
        std::vector<AnmKeyframe> Keyframes = {};
    };

    /** @brief Parses an ANM asset file.
     *
     * @param filename Absolute asset file path on the system.
     * @return Parsed ANM asset data as a `void` pointer.
     */
    std::shared_ptr<void> ParseAnm(const std::filesystem::path& filename);
}
