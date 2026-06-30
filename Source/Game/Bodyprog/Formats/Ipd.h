#pragma once

#include "Game/Bodyprog/Formats/Lm.h"
#include "Game/Bodyprog/Formats/Model.h"

namespace Silent::Game
{
    constexpr int IPD_HEADER_MAGIC = 20;

    /** @brief Ground material types. */
    enum s_GroundType
    {
        GroundType_Default = 0,
        GroundType_1       = 1,
        GroundType_2       = 2,
        GroundType_Grass   = 3,
        GroundType_4       = 4,
        GroundType_5       = 5,
        GroundType_6       = 6,
        GroundType_7       = 7,
        GroundType_8       = 8,
        GroundType_9       = 9,
        GroundType_10      = 10,
        GroundType_11      = 11,
        GroundType_None    = 12
    };

    /** @brief IPD file collision subcell split surface. */
    struct s_IpdCollSurface
    {
        q7_8 field_0;           // Relative X.
        q7_8 baseGroundHeight;  // Base ground height with slope unaccounted for?
        q7_8 field_4;           // Relative Z.
        u16  groundType    : 5; /** `e_GroundType` */
        u16  disableHeight : 3; /** `bool` */
        u16  field_6_8     : 3; // Causes special collision triggers to behave differently.
                                // The properties should be based upon a value set at
                                // `s_CollisionState::field_A0::s_0::field_8` and it shouldn't
                                // be bigger than 3.
                                // Side note: Maybe the bits number is incorrect as in reality
                                // it gives values up to 7 when it should have been `2` as it would
                                // only ever give a value up to 4, but some dev didn't paid attention
                                // and assigned 3 instead.
        u16  field_6_11    : 4; /** `bool` | Indicates the surface has a slope? */
        q7_8 tiltAngleX;
        q7_8 tiltAngleZ;
    };

    /** @brief IPD file collision subcell containing two split surfaces. */
    struct s_IpdCollSubcell
    {
        q7_8 field_0_0  : 14; // X. }
        u16  field_0_14 : 2;  // ID Values? } 
        q7_8 field_2_0  : 14; // Y. }
        u16  field_2_14 : 2;  // ID Values? } See `func_8006B318` definition for further explanation.
        q7_8 field_4;         // Z. }
        u8   splitVertexIdx0; // `s_IpdCollisionData::splitVertices` index.
        u8   splitVertexIdx1; // `s_IpdCollisionData::splitVertices` index.
        u8   surfaceIdx0;     // `s_IpdCollisionData::surfaces` index.
        u8   surfaceIdx1;     // `s_IpdCollisionData::surfaces` index.
    };

    typedef struct s_IpdCollisionData_18
    {
        u16      groundType    : 5; /** `e_GroundType` */
        u16      disableHeight : 3; /** `bool` */
        u16      field_0_8     : 4;
        u16      field_0_12    : 3;
        u16      field_0_15    : 1;
        SVECTOR3 offset;            /** Q7.8 */
        q7_8     field_8;
    };

    struct s_IpdCollSubcellRange
    {
        s16 field_0; // Base index into `s_IpdCollisionData::ptr_28`.
        s16 field_2; // Another index into `s_IpdCollisionData::ptr_28`.
    };

    /** @brief IPD file collision data. */
    struct s_IpdCollisionData
    {
        q23_8                  positionX;
        q23_8                  positionZ;
        u32                    splitVertexCount : 8; /** `splitVertices` size. */
        u32                    surfaceCount     : 8; /** `surfaces` size. */
        u32                    subcellCount     : 8; /** `subcells` size. */
        u32                    field_8_24       : 8; /** `ptr_18` size. */
        SVECTOR3*              splitVertices;        /** Split subcell vertices. Pairs define a split line. */
        s_IpdCollSurface*      surfaces;             /** Q7.8 | Split subcell surfaces. */
        s_IpdCollSubcell*      subcells;
        s_IpdCollisionData_18* ptr_18;
        q7_8                   subcellSize;
        u8                     subcellCountX;
        u8                     subcellCountZ;
        s_IpdCollSubcellRange* subcellRanges;
        u16                    field_24; // `ptr_24` size. `field_24`/`field_26` defined in ipd2obj but haven't seen used yet, might be size of `ptr_28`/`ptr_2C`.
        u16                    field_26; // `ptr_26` size.
        u8*                    ptr_28;   // Accessed as array of indices into `subcellCheckIdx` by `func_8006E53C`.
        u8*                    ptr_2C;         
        u8                     subcellCheckCount; // Directly related to `subcellCheckIdx`.
        s8                     __pad[3];
        u8                     subcellCheckIdx[256]; // Index container.
                                                    // This should be the same size as of the value assigned to
                                                    // `s_IpdCollisionData::subcellCount`, but instead is assigned the
                                                    // the max number that variable handles, which is 256 (same as a `char`).
    };

    /** @brief IPD file model info. */
    struct s_IpdModelInfo
    {
        u8             isGlobalPlm; // `false` if loaded from inside `IPD`, `true` if loaded from `*_GLB.PLM`.
        s8             __pad_1[3];
        u_Filename     name;
        s_ModelHeader* modelHdr;
    };

    /** @brief IPD file transformed model instance. TODO: Unsure. */
    struct s_IpdModelInstance
    {
        s_ModelHeader* modelHdr;
        MATRIX         mat; // TODO: Base transform matrix?
    };

    /** @brief IPD file model buffer. */
    struct s_IpdModelBuffer
    {
        u8                  modelInstanceCount; /** `modelInstances` size. */
        u8                  field_1;
        u8                  subcellCount;
        s8                  __pad_3;
        q7_8                minX; // } TODO: Cell range or subcell range?
        q7_8                maxX; // }
        q7_8                minZ; // }
        q7_8                maxZ; // }
        s_IpdModelInstance* modelInstances;
        SVECTOR*            field_10;         // Q7.8 | Pointer to unknown collision data. TODO: Wrong struct? See `Ipd_ChunkDraw`.
        SVECTOR*            subcellPositions; /** Q7.8 | XZ positions. TODO: Wrong struct? See `Gfx_ChunkSubcellVisibleCheck`. */
    };

    /** @brief IPD file header. */
    struct s_IpdHeader
    {
        u8                 magic;
        u8                 isLoaded; /** `bool` */
        s8                 cellX;
        s8                 cellZ;
        s_LmHeader*        lmHdr;
        u8                 modelCount;
        u8                 modelBufferCount;
        u8                 modelOrderCount;
        s8                 __pad_B[9];
        s_IpdModelInfo*    modelInfo;
        s_IpdModelBuffer*  modelBuffers;
        u8                 textureCount; // Should it be `u32`?
                                         // "`u8` - Relative pointer to textures list"
                                         // "`u32` - Relative pointer to object order"
        s8                 unk_1D[51];   // TODO: Actual texture data array?
        u8*                modelOrderList;
        s_IpdCollisionData collisionData;
    };
}
