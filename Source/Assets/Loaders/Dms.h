#pragma once

namespace Silent::Assets
{
    /** @brief DMS cutscene character transform keyframe. */
    struct DmsKeyframeCharacter
    {
        Vector3i Position = Vector3i::Zero; /** Q7.8 */
        Vector3i Rotation = Vector3i::Zero; /** Q3.12 */
    };

    /** @brief DMS cutscene camera keyframe. */
    struct DmsKeyframeCamera
    {
        Vector3i PositionTarget     = Vector3i::Zero; /** Q7.8 */
        Vector3i LookAtTarget       = Vector3i::Zero; /** Q7.8 */
        q3_12    ProjectionDistance = Q12(0.0f);      // TODO: Passed to `vcChangeProjectionValue`. Might be FOV related?
    };

    using DmsKeyframeType = std::variant<DmsKeyframeCharacter, DmsKeyframeCamera>;

    /** @brief Maps the inclusive playback frame range `[startFrameIdx, endFrameIdx]` to a single keyframe.
     * Used for playback frames where a character or camera has no movement and can remain set to a single keyframe.
     */
    struct DmsHoldRange
    {
        int StartFrameIdx = 0; /** Playback frame index. */
        int EndFrameIdx   = 0; /** Playback frame index. */
        int KeyframeIdx   = 0; /** DMS entry keyframe index. */
    };

    /** @brief DMS cutscene character or camera entry. */
    struct DmsEntry
    {
        std::string                  Name       = {};
        std::vector<DmsHoldRange>    HoldRanges = {}; /** Ranges of playback frames that map to a single keyframe, compressing repeated data. */
        std::vector<DmsKeyframeType> Keyframes  = {};
    };

    /** @brief DMS cutscene segment. */
    struct DmsSegment
    {
        int StartFrameIdx = 0; /** Playback frame index. */
        int FrameCount    = 0; /** Playback frame duration at 30 FPS. */
    };

    /** @brief DMS asset data. */
    struct DmsAsset
    {
        Vector3i                Origin           = Vector3i::Zero; /** Q23.8 | Cutscene origin added to relative character or camera positions. */
        std::vector<DmsSegment> Segments         = {};
        std::vector<DmsEntry>   CharacterEntries = {};
        DmsEntry                CameraEntry      = {};
    };

    /** @brief Parses a DMS asset file.
     *
     * @param filename Absolute asset file path.
     * @return Parsed DMS asset data as a `void` pointer.
     */
    std::shared_ptr<void> ParseDms(const stdfs::path& filename);
}
