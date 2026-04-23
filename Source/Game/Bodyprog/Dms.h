#pragma once

namespace Silent::Gam
{
    /** @brief DMS cutscene camera keyframe. */
    struct s_DmsKeyframeCamera
    {
        SVECTOR3 positionTarget; /** Q7.8 */
        SVECTOR3 lookAtTarget;   /** Q7.8 */
        s16      field_C[2];     // `field_C[1]` gets passed to `vcChangeProjectionValue`.
    };

    /** @brief DMS cutscene character transform keyframe. */
    struct s_DmsKeyframeCharacter
    {
        SVECTOR3 position; /** Q7.8 */
        SVECTOR3 rotation; /** Q7.8 */
    };

    /** @brief DMS cutscene entry. */
    struct s_DmsEntry
    {
        s16       keyframeCount_0;
        u8        svectorCount_2; /** `svectors_8` array size. */
        u8        field_3;        // Usually 0, but sometimes filled in, possibly junk data left in padding byte.
        char      name_4[4];      // First 4 chars of name. E.g. Code checks for "DAHLIA", file is "DAHL".
        SVECTOR3* svectors_8;     // Pointer to `SVECTOR3`s. Unknown purpose.
        union
        {
            s_DmsKeyframeCharacter* character;
            s_DmsKeyframeCamera*    camera;
        } keyframes_C;
    };

    /** @brief DMS cutscene interval */
    struct s_DmsInterval
    {
        s16 startKeyframeIdx;
        s16 frameCount; /** Frame duration at 30 FPS. */
    };

    /** @brief DMS cutscene file header. */
    struct s_DmsHeader
    {
        u8             isLoaded; /** `bool` */
        u8             characterCount;
        u8             intervalCount;
        u8             field_3; // Usually 0, but sometimes filled in.
        u32            field_4; // Unknown, correlates with DMS file size.
        s_DmsInterval* intervals;
        VECTOR3        origin; /** Q23.8 | Origin position. Gets added to character positions. */
        s_DmsEntry*    characters;
        s_DmsEntry     camera;
    };

    void DmsHeader_FixOffsets(s_DmsHeader* dmsHdr);

    void DmsEntry_FixOffsets(s_DmsEntry* entry, s_DmsHeader* dmsHdr);

    /** @unused? `volatile` needed for match. */
    s_DmsInterval* func_8008CA60(volatile s32 unused, s32 idx, s_DmsHeader* dmsHdr);

    void Dms_CharacterGetPosRot(VECTOR3* pos, SVECTOR3* rot, const char* charaName, q19_12 time, s_DmsHeader* dmsHdr);

    void Dms_CharacterGetStartPosRot(VECTOR3* pos, SVECTOR3* rot, const char* charaName, s32 time, s_DmsHeader* dmsHdr);

    s32 Dms_CharacterFindIdxByName(char* name, s_DmsHeader* dmsHdr);

    void Dms_CharacterGetPosRotByIdx(VECTOR3* pos, SVECTOR3* rot, s32 charaIdx, q19_12 time, s_DmsHeader* dmsHdr);

    void Dms_CharacterKeyframeInterpolate(s_DmsKeyframeCharacter* result, s_DmsKeyframeCharacter* frame0, s_DmsKeyframeCharacter* frame1, s32 alpha);

    /** @unused? Returns `96 * cotangent(angle / 2)`. Possibly camera/FOV related. */
    q3_12 func_8008CDBC(q3_12 angle);

    s32 Dms_CameraGetTargetPos(VECTOR3* posTarget, VECTOR3* lookAtTarget, u16* arg2, q19_12 time, s_DmsHeader* dmsHdr);

    /** @unused Returns whether any axis differs by more than 22.5 degrees (1/16 of full rotation). */
    bool func_8008CF54(SVECTOR3* rot0, SVECTOR3* rot1);

    s32 Dms_CameraKeyframeInterpolate(s_DmsKeyframeCamera* result, const s_DmsKeyframeCamera* frame0, const s_DmsKeyframeCamera* frame1, s32 alpha);

    void func_8008D1D0(s32* prevKeyframe, s32* nextKeyframe, q19_12* alpha, q19_12 time, s_DmsEntry* camEntry, s_DmsHeader* dmsHdr);

    u32 Dms_IntervalStateGet(q19_12 time, s_DmsHeader* dmsHdr);

    s32 func_8008D330(s32 arg0, s_DmsEntry* camEntry);

    s32 Math_LerpFixed12(s16 from, s16 to, q19_12 alpha);
}
