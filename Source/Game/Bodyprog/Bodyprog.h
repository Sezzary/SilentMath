#pragma once

#include "Game/Game.h"

#include "Game/Bodyprog/SoundEffects.h"
#include "Game/Bodyprog/EventFlags.h"
#include "Game/Bodyprog/View/VwSystem.h"
#include "Game/Main/FsQueue.h"

namespace Silent::Game
{
    constexpr int WORLD_OBJECT_COUNT_MAX = 29;

    constexpr int OPT_SOUND_VOLUME_MIN   = 0;
    constexpr int OPT_SOUND_VOLUME_MAX   = 128;
    constexpr int OPT_VIBRATION_DISABLED = 0;
    constexpr int OPT_VIBRATION_ENABLED  = 128;

    constexpr int  IPD_HEADER_MAGIC = 20;
    constexpr char LM_HEADER_MAGIC  = '0';
    constexpr int  LM_VERSION       = 6;

    constexpr int CHARA_FORCE_FREE_ALL = 0xFF; /** `Chara_Load` can force free already loaded models to make room for new ones. */

    /** @brief Compares 8-character filenames using `u32`. Similar to `strcmp`.
     *
     * @param a First filename.
     * @param b Second filename.
     * @return `true` if the filenames aren't equal, `false` otherwise.
     */
    #define COMPARE_FILENAMES(a, b)                                  \
        (((u_Filename*)(a))->u32[0] != ((u_Filename*)(b))->u32[0] || \
        ((u_Filename*)(a))->u32[1] != ((u_Filename*)(b))->u32[1])

    /** @brief Character spawn flags. */
    enum e_SpawnFlags
    {
        SpawnFlag_None = 0,
        SpawnFlag_0    = 1 << 0,
        SpawnFlag_1    = 1 << 1,
        SpawnFlag_2    = 1 << 2,
        SpawnFlag_3    = 1 << 3,
        SpawnFlag_4    = 1 << 4
    };

    // Used by `func_8006E490` and `func_8006E150`.
    typedef enum _OrientationFlags
    {
        OrientationFlags_None    = 0,
        OrientationFlags_InvertX = 1 << 0,
        OrientationFlags_InvertZ = 1 << 1,
        OrientationFlags_SwapXz  = 1 << 2
    } e_OrientationFlags;

    /** @brief Animation playback states. Returned by `Chara_AnimPlaybackStateGet`. */
    typedef enum _AnimPlaybackState
    {
        AnimPlaybackState_Blend   = -2,
        AnimPlaybackState_Invalid = -1, // Unsure.
        AnimPlaybackState_Active  = 0,
        AnimPlaybackState_End     = 1
    } e_AnimPlaybackState;

    /** @brief Background music flags. */
    typedef enum _BgmFlags
    {
        BgmFlag_Layer0    = 1 << 0,
        BgmFlag_Layer1    = 1 << 1,
        BgmFlag_Layer2    = 1 << 2,
        BgmFlag_Layer3    = 1 << 3,
        BgmFlag_Layer4    = 1 << 4,
        BgmFlag_Layer5    = 1 << 5,
        BgmFlag_Layer6    = 1 << 6,
        BgmFlag_Layer7    = 1 << 7,
        BgmFlag_KeepAlive = 1 << 8,
        BgmFlag_MuteAll   = 1 << 9
    } e_BgmFlags;

    /** @brief Background music track indices. */
    typedef enum _BgmTrackIdx
    {
        BgmTrackIdx_None = 0,
        BgmTrackIdx_1    = 1,
        BgmTrackIdx_2    = 2,
        BgmTrackIdx_3    = 3,
        BgmTrackIdx_4    = 4,
        BgmTrackIdx_5    = 5,
        BgmTrackIdx_6    = 6,
        BgmTrackIdx_7    = 7,
        BgmTrackIdx_8    = 8,
        BgmTrackIdx_9    = 9,
        BgmTrackIdx_10   = 10,
        BgmTrackIdx_11   = 11,
        BgmTrackIdx_12   = 12,
        BgmTrackIdx_13   = 13,
        BgmTrackIdx_14   = 14,
        BgmTrackIdx_15   = 15,
        BgmTrackIdx_16   = 16,
        BgmTrackIdx_17   = 17,
        BgmTrackIdx_18   = 18,
        BgmTrackIdx_19   = 19,
        BgmTrackIdx_20   = 20,
        BgmTrackIdx_21   = 21,
        BgmTrackIdx_22   = 22,
        BgmTrackIdx_23   = 23,
        BgmTrackIdx_24   = 24,
        BgmTrackIdx_25   = 25,
        BgmTrackIdx_26   = 26,
        BgmTrackIdx_27   = 27,
        BgmTrackIdx_28   = 28,
        BgmTrackIdx_29   = 29,
        BgmTrackIdx_30   = 30,
        BgmTrackIdx_31   = 31,
        BgmTrackIdx_32   = 32,
        BgmTrackIdx_33   = 33,
        BgmTrackIdx_34   = 34,
        BgmTrackIdx_35   = 35,
        BgmTrackIdx_36   = 36,
        BgmTrackIdx_37   = 37,
        BgmTrackIdx_38   = 38,
        BgmTrackIdx_39   = 39,
        BgmTrackIdx_40   = 40,
        BgmTrackIdx_41   = 41
    } e_BgmTrackIds;

    typedef enum _CollisionFlags
    {
        CollisionFlag_None = 0,
        CollisionFlag_0    = 1 << 0,
        CollisionFlag_1    = 1 << 1,
        CollisionFlag_2    = 1 << 2,
        CollisionFlag_3    = 1 << 3,
        CollisionFlag_All  = 0xFFFF
    } e_CollisionFlags;

    typedef enum _CollisionType
    {
        CollisionType_None = 0,
        CollisionType_Wall = 1,
        CollisionType_Unk2 = 2
    } e_CollisionType;

    /** @brief Map flags. */
    enum e_MapFlags
    {
        MapFlag_FourActiveChunks = 0,      /** Used by exterior maps. */
        MapFlag_OneActiveChunk   = 1 << 0,
        MapFlag_TwoActiveChunks  = 1 << 1,
        MapFlag_Interior         = 1 << 2,
        MapFlag_Unk3             = 1 << 3  /** @unused Unused map type `XXX` has this flag. */
    };

    /** @brief Used as index into `MAP_INFOS` array.
     * TODO: Add descriptions for which areas are included in each type?
    */
    typedef enum _MapType
    {
        MapType_THR = 0,
        MapType_SC  = 1,
        MapType_SU  = 2,
        MapType_SPR = 3,
        MapType_SPU = 4,
        MapType_RSR = 5,
        MapType_RSU = 6,
        MapType_APR = 7, /** @unused */
        MapType_APU = 8,
        MapType_ER  = 9,
        MapType_ER2 = 10,
        MapType_DR  = 11,
        MapType_DRU = 12,
        MapType_HP  = 13,
        MapType_HU  = 14,
        MapType_XXX = 15, /** @unused */

        MapType_Count
    } e_MapType;

    typedef enum _BoneHierarchy
    {
        BoneHierarchy_MultiModel = -3,
        BoneHierarchy_End        = -2
    } e_BoneHierarchy;

    typedef enum _EffectTextureFlags
    {
        EffectTextureFlag_None         = 0,
        EffectTextureFlag_Glass        = 1 << 1, /** Broken glass in the cafe Air Screamer cutscene. */
        EffectTextureFlag_WaterRefract = 1 << 2, /** Water waves and light reflection in the sewer. */
        EffectTextureFlag_Water        = 1 << 3,
        EffectTextureFlag_Fire         = 1 << 4,
        EffectTextureFlag_Ef           = 1 << 5, // TODO: Rename. Looks like stringy flesh?
        EffectTextureFlag_Blood        = 1 << 6,
        EffectTextureFlag_WarmTest     = 1 << 7  /** @unused Test lens flare? */
    } e_EffectTextureFlags;

    typedef enum _SpeedZoneType
    {
        SpeedZoneType_Slow   = 0,
        SpeedZoneType_Normal = 1,
        SpeedZoneType_Fast   = 2
    } e_SpeedZoneType;

    typedef enum _MainMenuEntry
    {
        MainMenuEntry_Load     = 0,
        MainMenuEntry_Continue = 1,
        MainMenuEntry_Start    = 2,
        MainMenuEntry_Option   = 3,
        MainMenuEntry_Extra    = 4, /** @unused The extra options menu may have been accessible via the main menu. */

        MainMenuEntry_Count    = 5
    } e_MainMenuEntry;

    typedef enum _MainMenuState
    {
        MenuState_Start              = 0,
        MenuState_Main               = 1,
        MenuState_LoadGame           = 2,
        MenuState_DifficultySelector = 3,
        MenuState_NewGameStart       = 4
    } e_MainMenuState;

    /** Used by `func_8003F654` to cast a specific field to the desired type. */
    typedef enum _PrimitiveType
    {
        PrimitiveType_None = 0,
        PrimitiveType_S8   = 1,
        PrimitiveType_U8   = 2,
        PrimitiveType_S16  = 3,
        PrimitiveType_U16  = 4,
        PrimitiveType_S32  = 5
    } e_PrimitiveType;

    typedef enum _LoadingScreenId
    {
        LoadingScreenId_None              = 0,
        LoadingScreenId_PlayerRun         = 1,
        LoadingScreenId_BackgroundTexture = 2,
        LoadingScreenId_StageString       = 3
    } e_LoadingScreenId;

    typedef enum _StaticModelLoadState
    {
        StaticModelLoadState_Invalid   = 0,
        StaticModelLoadState_Unloaded  = 1,
        StaticModelLoadState_Corrupted = 2, // Maybe wrong name for this.
        StaticModelLoadState_Loaded    = 3
    } e_StaticModelLoadState;

    /** SFX pair used for area loading (e.g. door opening and closing). */
    struct s_AreaLoadSfx
    {
        u16 sfx_0;
        u16 sfx_2;
    };

    // Exception, as one of the unidentified structs uses this.
    typedef struct _s_8002AC04
    {
        struct _s_8002AC04* field_0;

        union
        {
            u8 funcIdx_4; // `funcIdx_4` in structs below, needs to be part of those structs for match.

            struct
            {
                u8  funcIdx_4;
                u8  field_5;
                u16 field_6;
                s32 field_8;
                u32 field_C;
            } func_8008973C_data;
            struct
            {
                u8  funcIdx_4;
                u8  field_5;
                u16 field_6;
                s32 field_8;
                u32 field_C;
            } func_80089D0C_data;

            struct
            {
                u8                  funcIdx_4;
                u8                  field_5   : 7;
                u32                 field_5_7 : 1;
                u16                 field_6;
                struct _s_8002AC04* field_8;
                u32                 field_C;
            } func_80089BB8_data;

            struct
            {
                u8  funcIdx_4;
                u8  field_5;
                u16 field_6;
                u8* field_8;
                u32 field_C_0  : 16;
                u32 field_C_16 : 12;
                u32 field_C_28 : 3;
                u32 field_C_31 : 1;
            } func_80089DF0_data;
            struct
            {
                u8  funcIdx_4;
                u8  field_5_0 : 5;
                u32 field_5_5 : 1;
                u32 field_5_6 : 1;
                u32 field_5_7 : 1;
                u16 field_6;
                s32 field_8;
                u32 field_C;
            } func_80089A30_data;
        } field_4;
    } s_8002AC04;

    typedef struct
    {
        s32               id;
        s32               flags_4;
        s32               modelCount;
        //struct TMD_STRUCT models[1];
    } s_TmdFile;

    typedef struct
    {
        u8  unk_0;
        u8  field_1;
        u8  unk_2[18];
        s32 field_14;
    } s_800C45C8;

    typedef struct
    {
        char pad[12];
    } s_func_8005B424;

    // Something related to enemies attacks.
    typedef struct
    {
        union
        {
            q19_12 vx_0; // Displacement offset.
            struct
            {
                s16 field_0;
                s16 field_2;
            } s_0;
            struct
            {
                u16 field_0;
                u16 field_2;
            } s_1;
        } field_0;
        union
        {
            q19_12 vz_4;
            struct
            {
                s16 field_0;
                s16 field_2;
            } s_0;
        } field_4;
        q3_12 vy_8;
        u8    field_A;
        u8    field_B; // Flags?
        union
        {
            s32 field_0; // Timer.
            struct
            {
                q3_12 field_0; // Angle.
                q3_12 field_2; // Angle.
            } s_0;
            struct
            {
                u8 field_0;
                u8 field_1;
                u8 field_2;
                u8 field_3;
            } s_1;
            struct
            {
                q4_12 field_0;
                u16   field_2;
            } s_2;
        } field_C;
        union
        {
            q20_12 field_0;
            struct
            {
                s16 field_0;
                s16 field_2;
            } s_0;
            struct
            {
                s16 field_0;
                u8  field_2;
                u8  field_3;
            } s_1;
            struct
            {
                u8 field_0;
                u8 field_1;
                u8 field_2;
                u8 field_3;
            } s_2;
            struct
            {
                u16 field_0;
                u16 field_2;
            } s_3;
        } field_10;
    } s_MapHdr_field_4C;

    typedef struct _Collision
    {
        q19_12 groundHeight_0;
        q3_12  field_4;  // } Angles??
        q3_12  field_6;  // }
        s8     field_8;  // Count of something, maybe valid ground at probed points around center? Set to 0, 7, or 12.
    } s_Collision;

    struct s_CollisionQuery
    {
        VECTOR3 position;       // Q19.12    /* 0xC  */ q3_12   bottom;
        q3_12   top;
        q3_12   radius;         // Unsure. Set to cylinder radius.
        s8      collisionState; /** `e_CharaCollisionState` */
    };

    // Collision-related.
    typedef struct
    {
        s32        collisionState; /** `e_CharaCollisionState` */
        bool       field_4;
        q19_12     distance_8;
        SVECTOR    offset_C; // Q23.8
        DVECTOR_XZ direction_14;
        q23_8      positionX_18;
        q23_8      positionZ_1C;
        s32        field_20;
        s32        field_24;
        q7_8       field_28;      // Radius.
        q7_8       angleToTarget; // Wrong name. Top.
        q7_8       field_2C;      // Bottom.
    } s_func_8006ABC0;

    typedef struct
    {
        u8  unk_0[2];
        s16 field_2;
        u8  unk_4[2];
        s16 field_6;
    } s_func_8006CA18;

    typedef struct
    {
        u8  field_0;
        u8  field_1;
        s16 field_2;
        s32 field_4;
    } s_CollisionState_A8;

    typedef struct
    {
        s8      field_0;
        s8      unk_1;
        DVECTOR field_2; // Q3.12 | Angle constraint. X is min, Y is max. TODO: Don't use `DVECTOR` for this anymore.
    } s_CollisionState_44_0;

    typedef struct
    {
        s_CollisionState_44_0 field_0;
        s16                  field_6;
        s_CollisionState_44_0 field_8;
        s16                  field_E;
        s8*                  field_10[8];
        s_CollisionState_44_0 field_30;
        s16                  field_36;
    } s_CollisionState_44;

    // Function from lib_unk
    typedef struct _func_8009ECCC
    {
        struct _func_8009ECCC* field_0; // TODO: Not sure if these point to other struct type or same type.
        struct _func_8009ECCC* field_4;
        s32                    field_8;
        s32                    field_C;
        s_8002AC04*            field_10;
        u32                    field_14_0  : 16;
        u32                    field_14_16 : 8;
        u32                    field_14_24 : 7;
        u32                    field_14_31 : 1;
        u32                    field_18;
        u16                    field_1C;
        u16                    field_1E;
    } s_func_8009ECCC;

    typedef struct
    {
        q19_12 field_0; // X position.
        q19_12 field_4; // Z position.
        s32    field_8;
        s32    field_C;
        q19_12 field_10; // X
        q19_12 field_14; // Z
        s32    field_18; // X } Bounds?
        s32    field_1C; // X }
        s32    field_20; // Z }
        s32    field_24; // Z }
        q19_12 field_28; // Maybe XZ position.
        q19_12 field_2C; // Base height?
    } s_func_8006F338;

    typedef struct
    {
        s16 field_0;
        s16 field_2;
    } s_func_8006E490_20;

    typedef struct
    {
        s8                 unk_0[8];
        DVECTOR            field_8;
        s16                field_C;
        s16                field_E;
        s32                field_10; // } X
        s32                field_14; // } Z?
        s16                field_18;
        s8                 unk_1A[2];
        s32                field_1C; // Index into `field_20`.
        s_func_8006E490_20 field_20[2];
    } s_func_8006E490;

    /** @brief Character keyframe collision info. */
    typedef struct _Keyframe
    {
        s_CharaBox          box;
        s_CharaShapeOffsets shapeOffsets;
    } s_Keyframe;

    /** @brief 8-character string usually used for filenames. Can be compared via the `u32` field. */
    typedef union _Filename
    {
        char str[8];
        u32  u32[2];
    } u_Filename;

    /** @brief Struct used by many functions involved with GTE. Kept at `PSX_SCRATCH_ADDR` (possibly only temporarily). */
    typedef struct _GteScratchData
    {
        DVECTOR screenXy_0[90];
        u16     screenZ_168[18];

        s16 field_18C[99]; // The size changed from 150 due to the addition of `field_252`. Not sure if this is correct.
        u8  field_252[102];

        u8      field_2B8[200]; // Size likely incorrect.

        union
        {
            MATRIX field_0;
            struct
            {
                s32     field_0;
                s32     field_4;
                CVECTOR field_8;
                CVECTOR field_C;
                u8      field_10;
                u8      field_11;
                u8      field_12;
                u8      field_13;
                u8      field_14;
                u8      field_15;
                u8      field_16;
                u8      field_17;
                s32     field_18;
                s32     field_1C;
            } s_0;
        } field_380;

        s_Normal field_3A0;

        DVECTOR  screenPos_3A4;
        s32      depthP_3A8;
        SVECTOR  field_3AC;
        u8       unk_3B4[36];
        CVECTOR  field_3D8; // Written by `func_8005A42C`.
        DVECTOR  screenPos_3DC;
        s32      depthP_3E0;
        s16      rotMatrix_3E4[3][3]; // Truncated `MATRIX` without the `long t[3];` transfer vector?
    } s_GteScratchData;

    struct s_Primitive
    {
        u16 field_0;
        u16 field_2;
        u16 field_4;
        union
        {
            struct
            {
                u8 field_6_0  : 8;
                s8 field_6_8  : 7;
                u8 field_6_15 : 1; // `bool`
            } bits;

            u16 flags; // @hack `func_8005AC50` accesses `field_6_15` above with some weird shifts, haven't found how to make it work with bitfield yet.
        } field_6;
        u16 field_8;
        u16 field_A;
        u8  field_C[4];  // } Unknown type. `func_8005AC50` reads these as `s32`, but that breaks other funcs which use this struct. `s32` reads might have just been `memcpy`?
        u8  field_10[4]; // }
    };

    struct s_MeshHeader
    {
        u8           primitiveCount;
        u8           vertexCount;
        u8           normalCount;
        u8           unkCount;
        s_Primitive* primitives;
        DVECTOR*     verticesXy;
        s16*         verticesZ;
        s_Normal*    normals;
        u8*          unkPtr;
    };

    struct s_ModelHeader
    {
        u_Filename    name_0;
        u8            meshCount_8;
        u8            vertexOffset_9;
        u8            normalOffset_A;
        u8            field_B_0 : 1;
        u8            field_B_1 : 3; // Value used in `func_800571D0` switch.
        u8            field_B_4 : 2;
        u8            unk_B_6   : 2;
        s_MeshHeader* meshHdrs_C;
    };

    /** @brief Model texture. */
    struct s_Texture
    {
        s_FsImageDesc imageDesc;
        u_Filename    name;
        u32           queueIdx;
        s8            refCount;
    };

    struct s_Material
    {
        u_Filename name_0;
        s_Texture* texture_8;
        u8         field_C;
        u8         unk_D[1];
        u8         field_E;
        u8         field_F;
        u16        field_10;
        u16        field_12;
        union
        {
            u8  u8[2];
            u16 u16;
        } field_14;
        union
        {
            u8  u8[2];
            u16 u16;
        } field_16;
    };

    /** @brief LM file header. */
    struct s_LmHeader
    {
        u8             magic;    /** See `LM_HEADER_MAGIC`. */
        u8             version;  /** See `LM_VERSION`. */
        u8             isLoaded; /** `bool` */
        u8             materialCount;
        s_Material*    materials;
        u8             modelCount;
        s_ModelHeader* modelHdrs;
        u8*            modelOrder;
    };

    typedef struct _IpdCollisionData_10
    {
        s16 field_0;
        s16 field_2;
        s16 field_4;
        u16 field_6_0  : 5; // TODO: Might be using `s_IpdCollisionData_18` substruct here? Won't fit though.
        u16 field_6_5  : 3;
        u16 field_6_8  : 3;
        u16 field_6_11 : 4;
        u16 field_6_15 : 1;
        s16 field_8;
        s16 field_A;
    } s_IpdCollisionData_10;

    typedef struct
    {
        s16 field_0_0  : 14;
        u16 field_0_14 : 2;
        s16 field_2_0  : 14;
        u16 field_2_14 : 2;
        s16 field_4;
        u8  field_6;
        u8  field_7;
        u8  field_8;
        u8  field_9;
    } s_IpdCollisionData_14;

    typedef struct
    {
        s16 field_0; // Base index into `s_IpdCollisionData::ptr_28`.
        s8  __pad_2[2];
    } s_IpdCollisionData_20;

    typedef struct _IpdCollisionData_18
    {
        u16      field_0_0  : 5;
        u16      field_0_5  : 3;
        u16      field_0_8  : 4;
        u16      field_0_12 : 3;
        u16      field_0_15 : 1;
        SVECTOR3 vec_2;
        s16      field_8;
    } s_IpdCollisionData_18;

    typedef struct _IpdCollisionData
    {
        s32                    positionX;
        s32                    positionZ_4;
        u32                    field_8_0  : 8;
        u32                    field_8_8  : 8;
        u32                    field_8_16 : 8;
        u32                    field_8_24 : 8;
        SVECTOR3*              ptr_C;
        s_IpdCollisionData_10* ptr_10;
        s_IpdCollisionData_14* ptr_14;
        s_IpdCollisionData_18* ptr_18;
        q7_8                   field_1C;
        u8                     field_1E; // } Used as multipliers for `field_1C` in `func_8006B004`.
        u8                     field_1F; // }
        s_IpdCollisionData_20* ptr_20;
        u16                    field_24; // `field_24/``field_26` defined in ipd2obj but haven't seen used yet, might be size of `ptr_28`/`ptr_2C`.
        u16                    field_26;
        u8*                    ptr_28; // Accessed as array of indices into `field_34` by `func_8006E53C`.
        void*                  ptr_2C;
        u8                     field_30;
        s8                     __pad_31[3];
        u8                     field_34[256];
    } s_IpdCollisionData;

    // Node of some kind?
    typedef struct _IpdModelBuffer_C
    {
        s_ModelHeader* modelHdr;
        MATRIX         mat;
    } s_IpdModelBuffer_C;

    /** @brief IPD model buffer. */
    typedef struct _IpdModelBuffer
    {
        u8                  field_0; // Count of `field_C` entries.
        u8                  field_1;
        u8                  subcellCount;
        q7_8                minX; // } TODO: Cell range or subcell range?
        q7_8                maxX; // }
        q7_8                minZ; // }
        q7_8                maxZ; // }
        s_IpdModelBuffer_C* field_C;
        SVECTOR*            field_10;         // Pointer to unknown collision data.
        SVECTOR*            subcellPositions; /** XZ positions. TODO: Use different struct. */
    } s_IpdModelBuffer;

    typedef struct _IpdModelInfo
    {
        u8             isGlobalPlm; // `false` if loaded from inside `IPD`, `true` if loaded from `*_GLB.PLM`.
        u_Filename     modelName;
        s_ModelHeader* modelHdr;
    } s_IpdModelInfo;

    /** @brief IPD model file header. */
    typedef struct _IpdHeader
    {
        u8                 magic;
        u8                 isLoaded; /** `bool` */
        s8                 cellX_2;
        s8                 cellZ_3;
        s_LmHeader*        lmHdr_4;
        u8                 modelCount;
        u8                 modelBufferCount_9;
        u8                 modelOrderCount_A;
        s8                 unk_B[9];
        s_IpdModelInfo*    modelInfo_14;
        s_IpdModelBuffer*  modelBuffers_18;
        u8                 textureCount_1C; // Should it be `u32`?
                                            // "`u8` - Relative pointer to textures list"
                                            // "`u32` - Relative pointer to object order"
        s8                 unk_1D[51];
        u8*                modelOrderList_50;
        s_IpdCollisionData collisionData_54;
    } s_IpdHeader;

    // See: https://github.com/laura-a-n-n/silent-hill-museum/blob/main/ksy/sh1anm.ksy
    typedef struct _AnmBindPose
    {
        s8 parentBone;
        s8 rotationDataIdx;
        s8 translationDataIdx;
        s8 translationInitial[3];
    } s_AnmBindPose;

    /** @brief ANM file header. */
    struct s_AnmHeader
    {
        u16           dataOffset;
        u8            rotationBoneCount;
        u8            translationBoneCount;
        u16           keyframeDataSize; /** Size per keyframe. `(rotationBoneCount * 9) + (translationBoneCount * 3)`? */
        u8            boneCount;        /** Size of `bindPoses`. */
        u32           activeBones;      /** Holds bit field of bones to update. */
        u32           fileSize;
        u16           keyframeCount;
        u8            scaleLog2;
        u8            rootYOffset;
        s_AnmBindPose bindPoses[0];
    };

    typedef union
    {
        s16 field_0;
        struct
        {
            u8 field_0;
            u8 field_1;
        } s_field_0;
    } s_CollisionState_CC_C;

    typedef struct
    {
        DVECTOR_XZ field_0;
        DVECTOR_XZ field_4;
        u32        field_8;
        s32        field_C;
        s16        field_10;
        u8         unk_12[2];
        DVECTOR_XZ field_14;
    } s_CollisionState_CC_20;

    typedef struct
    {
        s_IpdCollisionData*   ipdCollisionData_0;
        u8                    field_4; // Index.
        u8                    field_5;
        SVECTOR3              field_6;
        s_CollisionState_CC_C  field_C;
        u8                    field_E;
        u8                    field_F;
        u8                    field_10;
        u8                    field_11;
        SVECTOR3              field_12;
        SVECTOR3              field_18;
        s8                    unk_1E[2];
        s_CollisionState_CC_20 field_20;
    } s_CollisionState_CC;

    typedef struct
    {
        u8                 field_0_0  : 8;
        s8                 field_0_8  : 1; // Something to do with collision. `bool` flag that states if there's a displacement?
        s8                 field_0_9  : 1;
        s8                 field_0_10 : 1;
        s8                 field_0_11 : 5;
        u16                field_2    : 16; // Collision flags.
        s_func_8006ABC0    field_4;
        s32                field_34;
        s16                field_38;
        s16                field_3A;
        s16                field_3C; // X?
        s16                field_3E; // Z?
        s8*                field_40;
        s_CollisionState_44 field_44;
        s32                field_7C;
        s32                field_80; // X
        s32                field_84; // Z
        s32                field_88; // X
        s32                field_8C; // Z
        s32                field_90; // `bool`?
        s32                field_94;
        union
        {
            DVECTOR_XZ vec_0;
            s32        field_0;
        } field_98;
        union
        {
            DVECTOR_XZ vec_0;
            s32        field_0;
        } field_9C;
        union
        {
            struct
            {
                u8                 field_0; // Start index for something.
                u8                 field_1;
                u8                 field_2;
                u8                 field_3;
                s_func_8006CA18*   field_4;
                s_CollisionState_A8 field_8[4];
            } s_0;
            struct
            {
                q7_8 field_0; // Set to absolute character bottom height.
                q7_8 field_2; // Set to absolute character top height.
                s16  field_4;
                u8   field_6;
                u8*  field_8;
                s8   unk_C[28];
            } s_1;
        } field_A0;
        u8                 box;
        u8                 unk_C9[1];
        s16                field_CA;
        s_CollisionState_CC field_CC;
        // TODO: May be incomplete. Maybe not, added the final padding based on `Collision_Get`.
    } s_CollisionState;

    /** @brief Global LM model. */
    typedef struct _GlobalLm
    {
        s_LmHeader* lmHdr;
        s32         fileIdx;
        s32         queueIdx;
    } s_GlobalLm;

    struct s_ModelInfo
    {
        s32            field_0; // Bone flags?
        GsCOORDINATE2* coord;
        s_ModelHeader* modelHdr;
        s32            modelIdx;
    };

    /** @brief IPD skeleton model bone. */
    typedef struct _Bone
    {
        s_ModelInfo modelInfo;
        s8          boneIdx;
    } s_Bone;

    /** @brief IPD skeleton model bone node. */
    typedef struct _LinkedBone
    {
        s_Bone              bone;
        struct _LinkedBone* next;
    } s_LinkedBone;

    typedef struct
    {
        u8            boneCount;
        u8            boneIdx;
        u8            field_2;
        s8            field_3;
        s_LinkedBone* bones_4;
        s_LinkedBone* bones_8;
        s_LinkedBone  bones_C[56];
    } s_Skeleton;

    typedef struct
    {
        s8  field_0;
        s8  field_1;
        s8  field_2;
        s8  field_3;
        s32 field_4[4];
    } s_800382B0;

    typedef struct
    {
        s16 field_0; // Flags?
    } s_8008D850;

    // or `s_AnimMetadata`?
    typedef struct _CharaAnimDataInfo
    {
        e_CharaId  charaId0_0;
        e_CharaId  charaId1_1;
        s_AnmHeader*   animFile0_4;
        s_AnmHeader*   animFile1_8;
        s32            animBufferSize1_C;
        s32            animBufferSize2_10;
        GsCOORDINATE2* npcBoneCoords;
    } s_CharaAnimDataInfo;

    /** Related to weapon attacks. Stats, SFX IDs, damange values, etc.? */
    typedef struct
    {
        q4_12         field_0;
        s16           field_2;
        u16           field_4; // Related to damage. Multiplier?
        s8            field_6; // Accessed by `func_8008BF84` as `u16`
        s8            unk_7;
        u8            charaId_8; // Accessed by `func_8008BF84` as `u16`
        e_CharaId field_9;
        u8            field_A; // Accessed by `func_8008BF84` as `u16`
        u8            field_B;
        u16           field_C;
        u8            field_E;  // Keyframe index offset?
        u8            field_F;  // Keyframe index offset?
        u8            field_10; // State.
        u8            field_11;
        u8            field_12; // SFX ID subgroup. Uses values 0-4.
        u8            __pad_13;
        u32*          unk_14; // Some pointer. All entries have the same value `D_800AD4C4`.
    } s_800AD4C8;

    /** @brief Collision point data. */
    typedef struct
    {
        VECTOR3     position; // Q19.12
        s_Collision collision_C;
        s32         field_18; // Count of points in circle?
    } s_CollisionPoint;

    struct s_800BCDA8
    {
        s8 field_0;
        s8 field_1;
        s8 field_2;
        s8 field_3;
    };

    /** @brief Speed zone. Defines a volume used for player speed modulation. */
    typedef struct _SpeedZone
    {
        s8    type; /** `e_SpeedZoneType` */
        q11_4 minX;
        q11_4 maxX;
        q11_4 minZ;
        q11_4 maxZ;
    } s_SpeedZone;

    /** @brief Water zone. Defines a volume used for visual water effects. */
    typedef struct _WaterZone
    {
        u8    isEnabled; /** `bool` */
        s16   illumination;
        q11_4 minX;
        q11_4 maxX;
        q11_4 minZ;
        q11_4 maxZ;
    } s_WaterZone;

    /** @brief Character model. */
    typedef struct
    {
        u8            charaId;  /** `e_CharaId` */
        u8            isLoaded; /** `bool` */
        s32           queueIdx;
        s_LmHeader*   lmHdr;
        s_FsImageDesc texture_C;
        s_Skeleton    skeleton;
    } s_CharaModel;

    /** @brief Loaded map info. */
    struct s_MapInfo
    {
        s16                plmFileIdx_0;
        char               tag_2[4];
        u8                 spawnFlags; /** `e_MapFlags` */
        const s_WaterZone* waterZones_8;
        const s_SpeedZone* speedZones_C;
    };

    // Rough name.
    typedef struct _WorldObjectMetadata
    {
        u_Filename name_0;
        s8         field_8;
        s8         lmIdx_9; /** Set to 2 when found in `g_Map.globalLm_138.lmHdr` and 3-6 if found in `g_Map.ipdActive_15C[i] (i + 3)`. */
    } s_WorldObjectMetadata;

    // Rough name.
    typedef struct _WorldObjectModel
    {
        s_ModelInfo           modelInfo;
        s_WorldObjectMetadata metadata;
    } s_WorldObjectModel;

    /** @brief Geometry-space world object to draw. */
    typedef struct _WorldObject
    {
        s_WorldObjectModel* model;
        q23_8               positionX  : 18;
        q23_8               positionY  : 14;
        q23_8               positionZ : 18;
        s32                 __pad_8_18 : 14;
        q21_10              rotationX  : 10;
        q19_12              rotationY  : 12;
        q21_10              rotationZ  : 10;
    } s_WorldObject;

    /** @brief World space trigger zone. */
    typedef struct _TriggerZone
    {
        u8  isEndOfArray : 1;  /** `bool` | Marks last entry. */
        s32 positionX    : 10; /** Meter steps. */
        s32 positionZ    : 10; /** Meter steps. */
        u32 sizeX        : 4;  /** Meter steps. */
        u32 sizeZ        : 4;  /** Meter steps. */
        u32 height       : 3;  /** Half-meter steps. Used to set `s_func_8006F338::field_2C` which is then copied by `func_8006F250`. */
    } s_TriggerZone;

    /** @brief Hand-held player item. */
    typedef struct _HeldItem
    {
        s32           itemId; /** `e_InventoryItemId` */
        s32           queueIdx;
        char*         textureName;
        s_FsImageDesc imageDesc;
        s_LmHeader*   lmHdr;
        s_Bone        bone;
    } s_HeldItem;

    /** @brief World GFX workspace.
     * TODO: Could be `s_RendererWork`? Will depend on where other data resides.
     * Will: `s_WorldModelWork` fits better, this is mainly responsible for handling model data.
     * `s_WorldEnvWork` should have this name as it is used for general GFX.
     */
    typedef struct _WorldGfxWork
    {
        s_MapInfo*        mapInfo;
        u8                useStoredPoint; /** `bool` */
        VECTOR3           ipdSamplePoint; /** Q19.12 | Used by IPD logic to sample which chunks to load or unload. */
        u8*               charaLmBuffer;
        s_CharaModel*     registeredCharaModels[Chara_Count];
        s_CharaModel      charaModels[CHARA_GROUP_COUNT];
        s_CharaModel      harryModel;
        s_HeldItem        heldItem; /** Item held by the player. */
        s_TriggerZone*    triggerZone;
        VC_CAMERA_INTINFO debugCameraInfo;
        s_LmHeader        itemLmHdr;
        u8                itemLmData[4096 - sizeof(s_LmHeader)]; // 4kb allocated for 2.75kb game files.
        s32               itemLmQueueIdx;
        s32               objectCount;                     /** `objects` size. */
        s_WorldObject     objects[WORLD_OBJECT_COUNT_MAX]; /** World objects to draw. */
    } s_WorldGfxWork;

    /** @brief IPD map geometry chunk. */
    struct s_IpdChunk
    {
        s_IpdHeader* ipdHdr;
        s32          queueIdx;
        s16          cellX;
        s16          cellZ;
        q19_12       distance0;
        q19_12       distance1;
        u8           materialCount;
        s8           __pad_15[3];
        s32          outsideCount;
    };

    typedef struct _IpdRow
    {
        s16 idx[16];
    } s_IpdColumn;

    struct s_ActiveTextures
    {
        s32        count_0;
        s_Texture* textures_4[10];
    };

    struct s_IpdTextures
    {
        s_ActiveTextures fullPage_0;
        s_ActiveTextures halfPage_2C;
        s_Texture        fullPageTextures_58[8];
        s_Texture        halfPageTextures_118[2];
    };

    typedef struct s_Map
    {
        s_IpdCollisionData collisionData_0; // Default chunk collision data?
        s32                texFileIdx_134;
        s_GlobalLm         globalLm_138;
        char               mapTag_144[4];
        s32                mapTagSize_148;
        s32                ipdFileIdx_14C;
        s_IpdHeader*       ipdBuffer_150;
        s32                ipdBufferSize_154;
        s32                ipdActiveSize_158;
        s_IpdChunk         ipdActive_15C[4];
        s_IpdColumn        ipdGrid_1CC[18];
        s8                 unk_40C[32];     // Could be one extra row in table above.
        s_IpdColumn*       ipdGridCenter_42C;
        s_IpdTextures      ipdTextures_430;
        q19_12             positionX_578;
        q19_12             positionX_57C;
        s32                cellX_580;
        s32                cellZ_584;
        bool               isExterior_588;
    };

    typedef struct
    {
        VECTOR3 field_0[2][1];
    } s_WorldEnvWork_84;

    /** @brief World environment workspace.
     *  Holds fog distances and ramps, lighting and color parameters,
     *  water zone references, and other per-map environmental data
     *  used during world rendering.
     */
    struct s_WorldEnvWork
    {
        u8            field_0;        // `bool`?
        u8            isFogEnabled_1; /** `bool` */
        u8            field_2;
        u8            field_3;        // Enviroment lighting.
        s_WaterZone*  waterZones_4;
        s32           screenBrightness_8;
        s32           fogNearDistance_C;
        q23_8         fogFarDistance_10; // "DrawDistanmce" in SHME, "has no effect when fog is disabled".
        s32           fogDepthShift_14;  // "FogThing1" from SHME. Affects the distance where fog begins.
        s32           fogIntensity_18;   // "FogThing2" from SHME. Affects the distance where fog begins.
        CVECTOR       fogColor_1C;
        s32           field_20;        // Map lighting.
        u8            field_24; // } RGB. Character color lighting.
        u8            field_25; // }
        u8            field_26; // }
        s8            unk_27;
        CVECTOR       worldTintColor_28;
        MATRIX        field_2C;
        s32           field_4C; // Light intensity in Q4?
        q3_12         lensFlareIntensity;
        q19_12        lightIntensity;
        SVECTOR       lightRotation; /** Q3.12 */
        VECTOR3       lightPosition; /** Q19.12 */
        SVECTOR       field_6C;      // Player current angles related to light?
        SVECTOR       field_74;
        SVECTOR       field_7C;
        s_WorldEnvWork_84 field_84[3];
        u8            fogRamp_CC[128]; // Fog-related values based on `fogNearDistance_C`/`fogFarDistance_10`.
        u16           field_14C;
    };

    typedef struct
    {
        u16            flags_0; // Collision flags.
        u8             triggerZoneCount_2;
        u8             unk_3;
        s_TriggerZone* triggerZones_4[20]; // Guessed size.
    } s_800C4478;

    typedef struct
    {
        u8      field_0; // `bool`?
        u8      field_1; // `bool`?
        u8      field_2; // `bool`?
        u8      field_3;
        s8      unk_4[4];
        s16     field_8;
        s16     field_A;
        VECTOR3 field_C; // Q19.12
        s8      unk_18[4];
        s32     field_1C;
        s32     field_20;
    } s_800C4818;

    typedef struct
    {
        u8  unk_0[4];
        s16 field_4;
        s16 field_6; // Keyframe index or time.
    } s_800C44F0; // Probable size: 8 bytes.

    /** @brief Character file info.
     * Holds file IDs of anim/model/texture for each `e_CharaId` along with some data used in VC camera code.
     */
    struct s_CharaFileInfo
    {
        s16            animFileIdx;
        s16            modelFileIdx;
        s16            textureFileIdx    : 16;
        q8_8           field_6           : 10;
        u16            materialBlendMode : 6; /** `e_BlendMode` */
        s_FsImageDesc* field_8;               // Extra texture pointer? Usually `nullptr` in `CHARA_FILE_INFOS`.
        u16            cameraAnchor  : 2;     /** `e_CameraAnchor` */
        q19_12         cameraOffsetY : 14;
    };

    /** @brief Used for normal credits screen. */
    typedef struct
    {
        s16  field_0;  /** Prim vertex X. */
        s16  field_2;  /** Prim vertex Y. */
        s16  field_4;  /** Width? */
        s8   field_6;  /** Height? */
        s8   field_7;  /** Blend flag. */
        s32  field_8;  /** Rgb24 color + code. */
        s16* field_C;  /** Points to 0x801E5C24, data size 400. */
        s32* field_10; /** Points to 0x801E5E24, data size 28, rgb24 + code. */
        u16  field_14; /** Tpage for `setDrawTPage`, calculated from `field_18` with `func_801E434C`. */
        s16  field_16; /** Clut XY. */
        u32  field_18; /** Texture UV. */
    } s_800AFE08;

    /** @brief Used for UFO ending credits screen. */
    typedef struct
    {
        s_800AFE08 sub_0;
        s32        field_1C;
        s16        field_20;
        s16        field_22;
        s32        field_24;
        s32        field_28;
        s32        field_2C;
        s32        field_30;
        s32        field_34;
        s32        field_38;
        s32        field_3C;
        s32        field_40;
        s32        field_44;
        s32        field_48;
        s32        field_4C;
        s32        field_50;
        s32        field_54;
    } s_800AFE24; // Size: 85

    /** @brief Contains X/Z coordinates and optional 4 bytes of data.
     * Map headers include an array of these, into which `s_EventData` includes an index. */
    struct s_MapPoint2d
    {
        q19_12 positionX_0;

        // Optional data.
        u32    mapIdx_4_0          : 5; /** `e_PaperMapIdx`? */
        u32    field_4_5           : 4;
        u32    loadingScreenId_4_9 : 3; /** `e_LoadingScreenId` */
        u32    field_4_12          : 4;
        q24_8  triggerParam0_4_16  : 8; // Usually a `Q8_ANGLE`
        u32    triggerParam1_4_24  : 8;

        q19_12 positionZ_8;
    };

    /** @brief Chracter spawn info. */
    struct s_SpawnInfo
    {
        q19_12 positionX;
        s8     charaId; /** `e_CharaId` */
        q0_8   rotationY;
        s8     spawnFlags;            /** `e_SpawnFlags` | Copied to `stateStep` in `s_Model`, with `controlState = 0`. */
        s32    gameDifficultyMin : 4; /** `e_GameDifficulty` | Minimum difficulty required for successful spawn. */
        q19_12 positionZ;
    };

    /** @brief Special map-specific Harry anim data. */
    typedef struct
    {
        s16   status; /** Packed anim status. See `s_ModelAnim::status`. */
        s16   status_2; /** Packed anim status. See `s_ModelAnim::status`. */
        q3_12 time;   /** Fixed-point anim time. */
        s16   keyframeIdx_6;
    } s_UnkStruct3_Mo; // Probable size: 8 bytes.

    /** Guessed based on in-debugger observation during gameplay.
     * Everything is inited to 0xFFFF and some data is written when the player is hit by monsters.
     * Might be more generic particles/decals struct.
     */
    typedef struct _BloodSplat
    {
        s16 field_0; // Maybe an array of `u16`s? Contains indices.
    } s_BloodSplat;

    typedef struct
    {
        u8    unk_0;
        u8    field_1;
        u8    field_2;
        u8    unk_3;
        u8    field_4;
        u8    field_5;
        u8    field_6;
        u8    unk_7;
        u8    unk_8[2];
        s16   field_A;
        u8    unk_C[2];
        s16   field_E;
        q3_12 field_10;
        u8    unk_12[4];
        s16   field_16;
        u8    unk_18[8];
        s32   field_20;
        s32   field_24;
    } s_MapOverlayHeader_5C;

    typedef struct
    {
        u8    field_0;
        u8    field_1;
        u8    field_2;
        u8    field_3;
        s16   field_4;
        s16   field_6;
        s16   field_8;
        s16   field_A;
        s16   field_C;
        s16   field_E;
        q3_12 field_10;
        s16   field_12;
        s32   field_14;
        s32   field_18;
        s32   field_1C;
    } s_MapOverlayHeader_7C;

    typedef struct
    {
        s32    field_0[4];
        s16    field_10[4];
        q19_12 field_18[4];
        u16    field_28[4];
        u8     field_30[4];
        q3_12  field_34[4]; // Y angles.
        u16    field_3C[4];
        u16    field_44[4];
        s16    field_4C[4];
        u16    field_54[4];
        u16    field_5C[4];
        q4_12  field_64[4];
        s16    field_6c[4];
        s16    field_74;
        s16    unk_76;
        u8     field_78;
    } s_MapOverlayHeader_94;

    /** TODO: `g_MapOverlayHeader` is part of the map overlay BIN files. Maybe should be moved to `maps/shared.h`.
     * If field has a comment that lists only certain map(s) it means all others set this field to 0.
     * func(?) means the signature is unknown and a default void() was selected for now.
     */
    struct s_MapOverlayHeader
    {
        s_MapInfo*             mapInfo;
        u8                     (*getMapRoomIdxFunc_4)(s32 x, s32 y); // Called by `Savegame_MapRoomIdxUpdate`.
        s8                     field_8;
        s32                    (*func_C)();
        void                   (*bgmEvent_10)(bool);
        s8                     bgmIdx_14;
        u8                     ambientAudioIdx_15; // Ambient file index from `g_AmbientVabTaskLoadCmds`.
        s8                     field_16;           // Set ambient tint and render distance.
                                                // A value of 3 sets the map to night.
                                                // A value of 2 will sets the map to use a tint used during the hallaway
                                                // intro.
                                                // Giving this and next variable a name will require further investigation
                                                // of `particle.c` code.
        s8                     field_17;           // Set weather to play. Rain, heavy rain, or snow.
        void                   (**loadingScreenFuncs_18)();
        s_MapPoint2d*          mapPointsOfInterest_1C;
        void                   (**mapEventFuncs_20)(); /** Points to array of event functions. */
        s_EventData*           mapEvents_24;
        GsCOORDINATE2*         field_28; // Bone coords of some kind.
        u8*                    loadableItems_2C;
        const char**           mapMessages_30; // Array of strings.
        s_AnimInfo*            harryMapAnimInfos_34; /** Map-specific anim infos for Harry (for anims 38+). */
        s_UnkStruct3_Mo*       field_38; // Array of 40?
        void                   (*worldObjectsInit_3C)(); // func(?).
        void                   (*worldObjectsUpdate_40)();
        void                   (*func_44)();
        void                   (*npcSpawnEvent_48)(); // func(?).
        s_MapHdr_field_4C*     unkTable1_4C; // Related to collision?
        s16                    unkTable1Count_50;
        s_BloodSplat*          bloodSplats_54;
        s16                    bloodSplatCount_58;
        s_MapOverlayHeader_5C* field_5C;
        void                   (*func_60)(s32, s32);
        //s32                    (*func_64)(POLY_FT4** poly, s32);
        //s32                    (*func_68)(POLY_FT4** poly, s32);
        void                   (*func_6C)(); // func(?).
        //s32                    (*func_70)(POLY_FT4** poly, s32);
        void                   (*func_74)(); // func(?).
        //s32                    (*func_78)(POLY_FT4** poly, s32);
        s_MapOverlayHeader_7C* field_7C;     // only map1_s01, map6_s04.
        void                   (*func_80)(); // func(?).
        //s32                    (*func_84)(POLY_FT4** poly, s32);
        s32*                   func_88; // only map0_s01, map7_s01.
        //s32                    (*func_8C)(POLY_FT4** poly, s32); // func(?) only map0_s01, map7_s01.
        //s32                    (*func_90)(POLY_FT4** poly, s32); // func(?) only map5_s00.
        s_MapOverlayHeader_94* field_94;                    // only map1_s02, map1_s03.
        //s32                    (*func_98)(POLY_FT4** poly, s32); // only map1_s02, map1_s03.
        void                   (*func_9C)();                // only map1_s02, map1_s03.
        void*                  ptr_A0; // M1S03 only, pointer to `s_800E3A40` array.
        //s32                    (*func_A4)(POLY_FT4** poly, s32); // func(?) only map1_s03.
        void                   (*func_A8)(); // func(?) only map6_s02.
        //s32                    (*func_AC)(POLY_FT4** poly, s32); // func(?) only map4_s03, map4_s05.
        //s32                    (*func_B0)(POLY_FT4** poly, s32); // func(?) only map4_s03, map4_s05.
        //s32                    (*func_B4)(POLY_FT4** poly, s32); // func(?) only map1_s03, map4_s05, map6_s01, map6_s02, map5_s01.
        void                   (*func_B8)(s_SubCharacter* chara, s_PlayerExtra* extra, GsCOORDINATE2* coords);
        void                   (*func_BC)(s_SubCharacter* chara, s_PlayerExtra* extra, GsCOORDINATE2* coords);
        void                   (*func_C0)(); // func(?).
        void                   (*playerMatchArmAnimDisable_C4)();
        void                   (*playerControlFreeze_C8)();
        void                   (*playerControlUnfreeze_CC)(bool);
        s32                    (*func_D0)(s32 playerExtraState, VECTOR3* vec, q3_12 angle, s32 vecCount); // 0x800C964C
        s32                    (*func_D4)(s32);                  // Assumed return type.
        void                   (*func_D8)();                     // Assumed return type.
        void                   (*playerAnimLock_DC)();
        void                   (*playerAnimIsLocked_E0)();
        s32                    (*playerAnimUnlock_E4)(s_SubCharacter*, s32); // Assumed return type.
        s64                    (*func_E8)(s_SubCharacter*);      // Is it really `s64`???
        s32                    (*playerMoveDistClear)();
        void                   (*playerMoveDistClear_F0)();
        void                   (*playerFallBackward_F4)();
        void                   (*func_F8)(); // func(?).
        void                   (*playerDamageFeetFront_FC)();
        void                   (*func_100)(); // func(?).
        void                   (*func_104)(); // func(?).
        s32                    (*func_108)();
        void                   (*func_10C)(); // func(?).
        void                   (*func_110)(); // func(?).
        void                   (*func_114)(); // func(?).
        void                   (*func_118)(); // func(?).
        void                   (*func_11C)(); // func(?).
        void                   (*func_120)(); // func(?).
        void                   (*func_124)(s_SubCharacter*); // Assumed return type.
        s32                    (*playerRunTimerReset_128)(s_SubCharacter* player);
        s32                    (*charaLock_12C)(s_SubCharacter* chara);
        void                   (*charaIsLockedCheck)(s_SubCharacter* chara);
        s32                    (*charaUnlock_134)(s_SubCharacter* chara);
        s32                    (*charaAnimPlaybackStateGet_138)(s_SubCharacter* chara);
        bool                   (*func_13C)(s_SubCharacter* chara, s32 arg1, VECTOR3* arg2In, s32 angleIn, s32 arg4); // `arg0` is `s_SubCharacter*`.
        void                   (*charaVisibleSet_140)(s_SubCharacter* chara);
        void                   (*charaInvisibleSet_144)(s_SubCharacter* chara);
        bool                   (*func_148)(s32 animStatus, s_SubCharacter* chara, s32 keyframeIdx0, s32 keyframeIdx1, s32 sfxId, s32 pitch);
        bool                   (*func_14C)(s32 animStatus, s_SubCharacter* chara, s32 keyframeIdx, s32 sfxId);
        s32                    (*charaAnimStartKeyframeIdxGet_150)(s_SubCharacter* chara);
        void                   (*func_154)(s_SubCharacter* chara);
        void                   (*func_158)(q19_12 x, q19_12 z); // only map1_s05, map1_s06.
        void                   (*func_15C)(); // func(?) only map5_s01.
        void                   (*func_160)(); // func(?) only map5_s01.
        void                   (*func_164)(); // func(?) only map5_s01.
        void                   (*particlesUpdate_168)(s32 unused, s32 mapId, s32);
        void                   (*enviromentSet_16C)(s8, u32);
        void                   (*func_170)(); // func(?).
        void                   (*func_174)(); // func(?).
        void                   (*particleHyperBlasterBeamDraw_178)(VECTOR3* vec0, q19_12* rotX, q19_12* rotY);
        void                   (*particleBeamDraw_17C)(const VECTOR3* from, const VECTOR3* to);
        void                   (*particleSoundStop_17C)();
        s32*                   windSpeedX_184;
        s32*                   windSpeedZ_188;
        s32*                   data_18C;
        s32*                   data_190;
        void                   (*charaUpdateFuncs_194[Chara_Count])(s_SubCharacter* chara, s_AnmHeader* anmHdr, GsCOORDINATE2* coords); /** Guessed params. Funcptrs for each `e_CharaId`, set to 0 for IDs not included in the map overlay. Called by `Game_NpcUpdate`. */
        s8                     charaGroupIds_248[CHARA_GROUP_COUNT]; /** `e_CharaId` values where if `s_SpawnInfo::charaId == Chara_None`, `charaGroupIds_248[0]` is used for `charaSpawns_24C[0]` and `charaGroupIds_248[1]` for `charaSpawns_24C[1]`. */
        s_SpawnInfo            charaSpawns_24C[2][16];               /** Array of character type/position/flags. `spawnFlags == 0` are unused slots? Read by `Game_NpcRoomInitSpawn`. */
        VC_ROAD_DATA           cameraPaths_3CC[100];
        s_TriggerZone          triggerZones_D2C[200];
    };

    typedef struct
    {
        s8 maxIdx;
        u8 selectedEntryIdx;
    } s_MapMsgSelect;

    typedef struct
    {
        u8 presetIdx1_0;
        u8 presetIdx2_1;
    } s_MapEffectsPresetIdxs;

    typedef struct
    {
        q23_8 field_0; // X position.                } Q8 according to `func_8006EE0C`?
        q23_8 field_4; // Y position.                }
        q7_8  field_8; // Z position, but why `s16`? }
        q7_8  field_A; // Y??
        q7_8  field_C; // Some kind of bound or threshold?
        s16   field_E;
    } s_RayState_6C; // Unknown size;

    typedef struct
    {
        s16 field_0;
        s16 field_2;
    } s_RayState_8C;

    /** @brief State for an in-progress ray trace. Contains pointers to active characters among other things. */
    typedef struct
    {
        s32              field_0;
        s16              field_4; // Collision flags.
        s16              field_6;
        q7_8             field_8; // Hit distance? `SHRT_MAX` if no valid hit.
        s8               unk_A[2];
        q19_12           field_C;  // } Q19.12 `VECTOR3`
        q19_12           field_10; // }
        q19_12           field_14; // }
        s8               unk_18[4];
        q7_8             field_1C; // Distance Z?
        s8               unk_1E[2];
        s_SubCharacter*  field_20;
        s16              field_24; // X } Q19.12
        s16              field_26; // Z }
        s32              field_28;
        VECTOR3          field_2C; // Q23.8
        s8               unk_38[4];
        s32              field_3C; // X  } Q23.8 `VECTOR3`?
        s32              field_40; // Y? }
        s32              field_44; // Z  }
        s8               unk_48[4];
        q7_8             field_4C; // X?
        q7_8             field_4E; // Z?
        SVECTOR          field_50; // Q23.8
        u16              field_58;
        s16              field_5A;
        s16              field_5C; // 
        s16              field_5E;
        s16              field_60;
        s8               unk_62[2];
        s_SubCharacter** characters_64; // Active characters?
        s32              characterCount_68;
        s_RayState_6C    field_6C;
        s32              field_7C;
        s32              field_80;
        u16              field_84;
        u8               unk_86[2];
        s32              field_88;
        s_RayState_8C    field_8C[1]; // Unknown size.
    } s_RayState;

    /** @brief Ray trace line of sight info. */
    struct s_RayTrace
    {
        s8              hasHit; /** `bool` */
        u8              field_1;
        VECTOR3         target; /** Q19.12 */
        s_SubCharacter* character;
        q19_12          field_14; // Hit distance X?
        q19_12          field_18; // Hit distance Z?
        q3_12           field_1C; // Angle? Counter??
    };

    struct s_CollisionResult
    {
        VECTOR3 offset_0; // Q19.12
        q19_12  field_C;  // Absolute ground height? Might be using `s_Collision` substruct?
        s16     field_10;
        s16     field_12;
        s8      field_14; // Count of something? 12 is significant.
        s32     field_18;
    };

    typedef struct
    {
        DVECTOR  screenXy_0[90];
        s16      screenZ_168[18];
        s16      field_18C[72];
        s32      field_21C[39]; // Used as `VECTOR3`?
        u8       field_2B8[200];
        MATRIX   field_380;
        s_Normal field_3A0;
        DVECTOR  screenPos_3A4;
        s32      depthP_3A8;
        SVECTOR  field_3AC;
        u8       unk_3B4[36];
        CVECTOR  field_3D8;

        // Different functions access different data at 0x3DC onwards.
        // Union works for it, but also possible those functions just took different `s_GteScratchData` structs.
        union
        {
            struct
            {
                DVECTOR screenPos_3DC;
                s32     depthP_3E0;
                MATRIX  field_3E4;
            } vertex;

            struct
            {
                s_Normal field_3DC;
                SVECTOR  field_3E0[3];
            } normal;

            struct
            {
                u8  field_0;
                u8  field_1;
                u8  field_2;
                u8  field_3;
                u8  field_4;
                u8  field_5;
                u8  field_6;
                u8  field_7;
                s32 field_8;
            } s_1;
        } u;
    } s_GteScratchData2;

    // Something for inventory items.
    typedef struct
    {
        q3_12   positionY;
        q3_12   field_2; // Move dist?
        q3_12   field_4; // Angle.
        s16     field_6;
        s16     field_8;
        s8      __pad_A[2];
        SVECTOR field_C;  // Q3.12 | Offset?
        SVECTOR position; /** Q3.12 */
    } s_800AE204;

    typedef struct
    {
        s_800AE204* ptr_0;
        u8          count_4;
        u8          unk_5;
        q7_8        field_6;
        u8          field_8; // UV0
        u8          field_9; // UV1
        u8          field_A; // UV2
        u8          field_B; // UV3
    } s_800AE4DC;

    typedef struct
    {
        u8 field_0;
        u8 field_1;
        u8 field_2;
        u8 field_3; // Map marking sprite height?
    } s_800AE8A0_0;

    typedef struct
    {
        s8 field_0;
        s8 field_1;
        u8 field_2;
        u8 field_3;
    } s_800AE8A0_4;

    /** Paper map marking graphic data? */
    typedef struct
    {
        //POLY_FT4*    field_0;
        s_800AE8A0_0 field_4;
        s_800AE8A0_4 field_8;
        s32          field_C; // Count.
    } s_func_80068E0C;

    typedef struct
    {
        //POLY_FT4* field_0;
        SVECTOR   field_4; // Q23.8
        MATRIX    field_C;
        VECTOR3   field_2C; // Q23.8
        s32       field_38;
        DVECTOR   field_3C;
        s32       field_40;
        DVECTOR   field_44;
        DVECTOR   field_48;
    } s_func_80065B94;

    typedef struct
    {
        //POLY_GT4* field_0;
        MATRIX    field_4;
        SVECTOR   field_24[3];
        VECTOR3   field_3C; // Q19.12
        s32       field_48;
        DVECTOR   field_4C;
        DVECTOR   field_50;
        DVECTOR   field_54;
        DVECTOR   field_58;
        s32       field_5C;
        s32       field_60;
        s32       field_64;
        s32       field_68;
        s32       field_6C;
        s32       field_70;
        s32       field_74;
        s32       field_78;
    } s_func_80066184;

    typedef struct
    {
        u8  field_0;
        u8  field_1;
        s16 field_2; // } XYZ? X and Y swapped?
        s32 field_4; // }
        s32 field_8; // }
    } s_800C42E8;

    typedef struct
    {
        s16 field_0;
        s16 field_2;
        s16 field_4;
        s16 field_6;
        u16 field_8;
        u16 field_A;
        u8  field_C;
    } s_800C4418;

    typedef struct
    {
        VECTOR3 field_0; // Q23.8 | Position.
        MATRIX  field_C;
        s32     field_2C;
        DVECTOR field_30;
        q3_12   field_34[24];
        q3_12   field_64[24];
        q3_12   field_94[24];
        s16     field_C4;
        s16     field_C6;
        s16     box;
        s16     field_CA;
        s16     field_CC;
        s16     field_CE;
        s32     field_D0;
        s32     cylinder;
        s32     field_D8;
        s16     field_DC[4];
        s16     field_E4[4];
        union
        {
            DVECTOR field_0[4]; // Q19.12
            s32     raw_0[4];
        } u_field_EC;
        union
        {
            DVECTOR field_0[4];
            s32     raw_0[4];
        } u_field_FC;         // Q3.12 | Positions or offsets.
        q19_12  field_10C[4]; // X offsets?
        q19_12  field_11C[4]; // Z offsets?
    } s_func_8005E89C;

    typedef struct
    {
        s_func_8005E89C field_0;
        PACKET*         field_12C;
        CVECTOR         field_130;
        CVECTOR         field_134;
        SVECTOR         field_138;
        s32             field_140;
        DVECTOR         field_144;
        u16             field_148;
        u8              unk_14A[2];
        s32             field_14C;
        s32             field_150;
        s32             field_154;
        s32             field_158;
        s32             field_15C;
        s32             field_160;
        s32             field_164;
        s32             field_168;
        s32             field_16C;
        s32             field_170;
        s32             field_174;
    } s_func_80060044;

    typedef struct
    {
        s_func_8005E89C field_0;
        CVECTOR         field_12C;
        CVECTOR         field_130;
        SVECTOR         field_134;
        SVECTOR         field_13C;
        SVECTOR         field_144;
        VECTOR3         field_14C;
        s32             field_158;
        s32             field_15C;
        s32             field_160;
        s32             field_164;
        s32             field_168;
        DVECTOR         field_16C;
        s32             field_170;
        s32             field_174;
        q19_12          field_178;
        s32             field_17C;
        s32             field_180;
        s32             field_184;
        s32             field_188;
        s32             field_18C;
        s32             field_190;
        s32             field_194;
        s32             field_198;
        s32             field_19C;
        s32             field_1A0;
    } s_func_800611C0;

    typedef struct
    {
        s_func_8005E89C field_0;
        CVECTOR         field_12C;
        CVECTOR         field_130;
        SVECTOR         field_134[25];
        VECTOR3         field_1FC; // Q19.12
        q20_12          field_208;
        s32             field_20C;
        s32             field_210;
        s32             field_214[25];
        DVECTOR         field_278[25];
        s32             field_2DC;
    } s_func_80062708;

    typedef struct
    {
        s_func_8005E89C field_0;
        SVECTOR field_12C;
        s8      unk_138[8];
        DVECTOR field_13C;
        s32     field_140;
        s32     field_144;
    } s_func_80064FC0;

    typedef struct
    {
        //POLY_G4*  field_0;
        //DR_TPAGE* field_4;
        SVECTOR   field_8[3];
        MATRIX    field_20;
        VECTOR3   field_40; // Q23.8
        s32       field_4C;
        s16       field_50;
        s32       field_54[3];
        s16       field_60[3];
        s32       field_68;
    } s_func_800652F4;

    typedef struct
    {
        s_func_8005E89C field_0;
        MATRIX          field_12C;
        SVECTOR         field_14C[3];
        SVECTOR         field_164;
        VECTOR          field_16C[3];
        VECTOR          field_19C;
        VECTOR          field_1AC; // Q27.4
        s32             field_1BC;
        s32             field_1C0;
        s32             field_1C4;
        s32             field_1C8;
        DVECTOR         field_1CC;
        s16             field_1D0;
        u8              unk_1D2[2];
        s32             field_1D4; // Count.
        s32             field_1D8;
        s32             field_1DC;
        s32             field_1E0;
        s32             field_1E4;
        q19_12          field_1E8;
    } s_func_80063A50;

    typedef struct
    {
        s_func_8005E89C field_0;
        PACKET*         field_12C;
        CVECTOR         field_130;
        CVECTOR         field_134;
        SVECTOR         field_138;
        u8              unk_140[16];
        s32             field_150;
        DVECTOR         field_154;
        u16             field_158;
        u16             field_15A;
        u8              field_15C;
        s32             field_160;
        s32             field_164;
        s32             field_168;
    } s_func_80064334;

    typedef struct
    {
        s_func_8005E89C field_0;
        s_Collision     field_12C;
        MATRIX          field_138;
        SVECTOR         field_158[4];
        VECTOR          field_178[4];
        CVECTOR         field_1B8;
        CVECTOR         field_1BC;
        s32             field_1C0;
        s32             field_1C4;
        s32             field_1C8;
        s32             field_1CC;
        s32             field_1D0;
        DVECTOR         field_1D4;
    } s_func_800CD1F8;

    typedef struct
    {
        MATRIX  field_0;
        SVECTOR field_20;
        VECTOR  field_28; // Q27.4
        s32     field_38;
        s32     field_3C[5];
        s32     field_50;
    } s_func_8006342C;

    extern s_FsImageDesc g_MainImg0; // 0x80022C74 - TODO: Part of main exe, move to `main/` headers?

    extern const s_MapInfo MAP_INFOS[MapType_Count];

    extern char D_80028544[16];

    //extern RECT D_80028A20;

    extern const s_AnimInfo D_80028B94[];

    extern s_800C44F0 D_800294F4[];

    extern const s_AnimInfo* D_800297B8;

    extern u_Filename D_8002B2CC;

    extern s_FsImageDesc g_Font16AtlasImg;
    extern s_FsImageDesc g_KonamiLogoImg;
    extern s_FsImageDesc g_KcetLogoImg;
    extern s_FsImageDesc D_800A900C;
    extern s_FsImageDesc g_TitleImg;
    extern s_FsImageDesc g_PaperMapImg;
    extern s_FsImageDesc g_PaperMapMarkingAtlasImg;
    extern s_FsImageDesc g_ItemInspectionImg;
    extern s_FsImageDesc g_LoadingScreenImg;
    extern s_FsImageDesc g_ControllerButtonAtlasImg;
    extern s_FsImageDesc g_BrightnessScreenImg0;
    extern s_FsImageDesc g_BrightnessScreenImg1;
    extern s_FsImageDesc g_DeathTipImg;
    extern s_FsImageDesc g_HealthPortraitImg;
    extern s_FsImageDesc g_InventoryKeyItemTextureImg;
    extern s_FsImageDesc g_FirstAidKitItemTextureImg;

    /** Some intentory item texture (`ITEM/TIM07.TIM`). */
    extern s_FsImageDesc D_800A9074;

    extern s_FsImageDesc D_800A907C;

    extern s_FsImageDesc D_800A9084;

    extern s_FsImageDesc D_800A908C;

    extern s_FsImageDesc D_800A9094;

    extern s_FsImageDesc g_Font24AtlasImg; // 0x800A909C

    /** Array containg file IDs used for each `e_CharaId`, used in `Fs_QueueStartReadAnm`. */
    extern s_CharaFileInfo CHARA_FILE_INFOS[Chara_Count]; // 0x800A90FC

    extern s_StructUnk3 D_800A952C;

    extern s32 D_800A9A20;

    /** X. */
    extern s32 D_800A9A24;

    /** Z. */
    extern s32 D_800A9A28;

    /** Loaded NPC types in memory and their `g_CharaTypeAnimInfo` indices. */
    extern s8 g_CharaAnimInfoIdxs[Chara_Count];

    extern s32 D_800A9EB0;

    extern s_FsImageDesc D_800A9EB4;

    extern s_FsImageDesc D_800A9EBC;

    extern s_FsImageDesc D_800A9EC4;

    /** @brief Stores a loaded character's animation data information. */
    extern s_CharaAnimDataInfo g_CharaTypeAnimInfo[];

    extern s32 D_800A9938;

    extern u8 D_800A9944[];

    /** Player anim info? */
    extern s_AnimInfo D_800A998C;

    /** Current player anim index store? */
    extern u8 D_800A9990;

    // Likely declared as static inside the function that uses it.
    extern s32 D_800A99A0;

    /** Relative file offset for map texture? */
    extern s8 D_800A99B5;

    extern s32 D_800A9A68;

    /** Counts the amount of times that demos has been play in the current game session. */
    extern s8 g_Demo_ReproducedCount;

    extern s32 g_MainMenu_NewGameSelectedDifficultyIdx;

    extern RECT D_800A9A6C; // `RECT<320, 256, 160, 240>`, only used in `SysState_Fmv_Update`?

    extern s32 D_800A9ECC;

    extern s32 D_800A9ED0;

    extern s32 D_800A9ED4;

    extern s32 D_800A9ED8;

    extern s32 D_800A9EDC;

    extern s32 D_800A9EE0;

    extern s32 D_800A9EE4;

    extern s32 D_800A9EE8;

    extern s32 D_800A9EEC;

    extern s32 D_800A9EF0;

    extern s32 D_800A9EF4;

    extern s32 D_800A9EF8;

    extern s32 D_800A9EFC;

    extern s32 D_800A9F00;

    extern s32 D_800A9F04;

    extern s32 D_800A9F08;

    extern s32 D_800A9F0C;

    extern s32 D_800A9F10;

    extern s32 D_800A9F14;

    extern s32 D_800A9F18;

    extern s32 D_800A9F1C;

    extern s32 D_800A9F20;

    extern s32 D_800A9F28;

    extern s32 D_800A9F2C;

    extern s32 D_800A9F30;

    extern s32 D_800A9F34;

    extern s32 D_800A9F38;

    extern s32 D_800A9F3C;

    extern s32 D_800A9F40;

    extern s32 D_800A9F44;

    extern s32 D_800A9F48;

    extern s32 D_800A9F4C;

    extern s32 D_800A9F50;

    extern s32 D_800A9F54;

    extern s32 D_800A9F58;

    extern s32 D_800A9F60;

    extern s32 D_800A9F64;

    extern s32 D_800A9F68;

    extern s32 D_800A9F6C;

    extern s32 D_800A9F74;

    extern s32 D_800A9F78;

    extern s32 D_800A9F7C;

    extern s_MapEffectsPresetIdxs D_800A9F80;

    extern s_MapEffectsPresetIdxs D_800A9F84;

    extern s_MapEffectsPresetIdxs D_800A9F88;

    extern s_MapEffectsPresetIdxs D_800A9F8C;

    extern s_MapEffectsPresetIdxs D_800A9F98;

    extern u32 D_800A9FB0;

    extern s32 D_800A9FB4[];

    extern u8 D_800AA604[41][16];

    extern u8 D_800AE185;

    extern u8 D_800AE186;

    extern u8 g_SysState_GameOver_TipIdx;

    /** `e_InventoryItemId` | related to displaying items. */
    extern u8 D_800AE187;

    extern s16 D_800AE1A8;

    extern s_800AE204 D_800AE204[26];

    extern s32 g_Items_PickupAnimState;

    extern q19_12 g_Items_PickupScale;

    extern s_800AE4DC D_800AE4DC[3];
    extern SVECTOR    D_800AE500[4];

    extern s16 D_800AE520[];

    extern s16 D_800AE564[];

    extern s16 D_800AE5CC[];

    extern s16 D_800AE5F0[];

    extern u8 D_800AE700[];

    /** `[1]` contains relative paper map file indices. */
    extern u8 D_800AE740[][2];

    extern s32 D_800AE770;

    extern q3_12 g_Player_FlexRotationY;

    extern q3_12 g_Player_FlexRotationX;

    /** Related to player. */
    extern u8 D_800AF220;

    /** @brief Last weapon selected. While it is being assigned the value of
     * `g_SysWork::playerCombat::weaponAttack` this time it is used to determine
     * the last weapon used in order to load the required animation data.
     */
    extern s32 g_Player_LastWeaponSelected;

    extern s_AnimInfo HARRY_BASE_ANIM_INFOS[]; // Maybe part of bigger struct. 0x800AF228

    extern s16 D_800AF506;

    extern s16 D_800AF526;

    extern s16 D_800AF5C6;

    extern s16 D_800AF624;

    /** Keyframe index. */
    extern s16 D_800AF626;

    /** Generic collision global used by high-level getters. */
    extern s_CollisionPoint g_CollisionPoint;

    extern u8 D_800AFD04;

    extern u8 D_800AFD05;

    extern const s_AreaLoadSfx SFX_PAIRS[25];

    extern s32 D_800AFD3C;

    extern s32 D_800AFD44;

    extern s32 D_800AFD60;

    extern s32 D_800AFD9C;

    extern SVECTOR D_800AFDB0;

    extern s32 D_800AFDEC;

    extern PACKET D_800BFBF0[2][0xA10];

    extern u8 g_Items_GunsMaxLoadAmmo[36]; // Max loaded ammo that a weapon can hold. 0x800AD4A0

    extern const char* INVENTORY_ITEM_NAMES[];

    extern const char* g_ItemDescriptions[];

    extern s_FsImageDesc D_800A90A4;

    extern s_FsImageDesc D_800A90B4;

    extern s_SubCharacter D_800BA00C; // Often passed to `func_800700F8`, might not be full `s_SubCharacter`?

    extern u8 D_800BC74F;

    extern u16 D_800BCCB0;

    extern u32 D_800A999C;

    extern u16 D_800BCCB2;

    /** @brief Packed screen fade effect data. Uses `e_ScreenFadeState`.
     *
     * Data is packed as follows:
     * State as `e_ScreenFadeState`: bits 0-2.
     * Is white:                     bit 3.
     *
     * TODO: Reference enum entries for details below.
     * 0-1   - Nothing.
     * 2-5   - Fades to black and keeps the screen black.
     * 6-7   - Fades from black.
     * 10-15 - Fades to white and keeps the screen white.
     * 16    - Fades to black.
     */
    extern s32 g_Screen_FadeStatus;

    extern s32 D_800BCD5C;

    extern const char D_80025320[];
    extern const char D_8002532C[];
    extern const char D_80025338[];
    extern const char D_80025344[];
    extern const char D_80025350[];
    extern const char D_8002535C[];
    extern const char D_80025368[];

    extern s_MapMsgSelect g_MapMsg_Select;

    extern s8 g_MapMsg_SelectCancelIdx;

    extern s32 D_800BCD84;

    /** Related to main menu fog randomization. */
    extern s8* D_800BCDE0; // Type assumed.

    /** Angles. */
    extern s16 D_800BCDE8[8];

    extern u16 g_CollisionFlags;

    extern s_WorldGfxWork g_WorldGfxWork;

    extern s_IpdCollisionData* D_800C1010[4];

    extern s_Map g_Map;

    extern s8* D_800C15B0;

    extern s8 D_800C15B4;

    extern s16 D_800C16A4;

    extern s16 D_800C391E;

    extern s8 D_800C39A0;

    /** Functions from `bodyprog_8003AB28` access `g_WorldEnvWork` as constant, but some functions from `bodyprog_80055028` write to it.
     * It appears that g_WorldEnvWork is intended to be defined inside `bodyprog_80055028` as writable and declared as read-only (`const`) outside of it.
     */
    // extern s_WorldEnvWork g_WorldEnvWork;

    extern GsCOORDINATE2* D_800C42B8;

    extern VECTOR3 D_800C42C0;

    extern VECTOR3* D_800C42CC;

    extern u16 D_800C42D0;

    extern u16 D_800C42D2;

    extern s_800C42E8 D_800C42E8[24];

    extern s16 D_800C4408;

    extern GsCOORDINATE2* D_800C440C;

    extern GsCOORDINATE2* D_800C4410;

    /** Flags. */
    extern s8 D_800C4414;

    extern s_800C4418 D_800C4418;

    extern s16 D_800C4454;

    // emoose: Also works: `extern u16 D_800C4478[];`, `arg0->field_4 = D_800C4478[0];`.
    // Didn't see any array accesses in Ghidra though, struct might be more likely.
    extern s_800C4478 D_800C4478;

    extern s8 D_800C447A;

    extern s16 D_800AEEDC[][2]; // Type assumed.

    extern s16 D_800AEF78[][2]; //  Type assumed.

    extern s16 D_800AF014[];

    extern s16 D_800AF04C[];

    extern s16 D_800AF070[]; // Type assumed.

    extern s16 D_800AF1FC[]; // Type assumed.

    extern u8 D_800C4606;

    extern s_Collision D_800C4620;

    extern VECTOR3 D_800C4640[2][8];

    /** Y angles. */
    extern q3_12 D_800C4700[8];

    /** Times. */
    extern q19_12 D_800C4710[];

    /** Q19.12 */
    extern VECTOR3 D_800C4788[];

    extern VECTOR3 D_800C47B8;

    extern VECTOR3 D_800C47C8[];

    extern VECTOR3 D_800C47E8;

    extern s_RayTrace D_800C47F8;

    extern s_800C4818 D_800C4818;

    extern s32 D_800C48F0;

    extern u8 D_800AD480[24];

    /** Weapon attacks. */
    extern s_800AD4C8 D_800AD4C8[70];

    extern s_MapOverlayHeader g_MapOverlayHeader; // 0x800C957C

    extern s16 SQRT[100];
}
