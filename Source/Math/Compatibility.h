#pragma once

#include "Math/Objects/EulerAngles.h"
#include "Math/Objects/Matrix.h"
#include "Math/Objects/Vector2i.h"
#include "Math/Objects/Vector3i.h"

namespace Silent::Math
{
    constexpr int LINE_VERTEX_COUNT = 2;
    constexpr int RECT_VERTEX_COUNT = 4;
    constexpr int BOX_VERTEX_COUNT  = 8;

    /** @brief Model primitive material flags. */
    enum e_MaterialFlags
    {
        MaterialFlag_None = 0,
        MaterialFlag_0    = 1 << 0,
        MaterialFlag_1    = 1 << 1,
        MaterialFlag_2    = 1 << 2
    };

    /** @brief Semi-transparency blend modes. */
    enum e_BlendMode
    {
        BlendMode_Average     = 0,
        BlendMode_Additive    = 1,
        BlendMode_Subtractive = 2
    };

    /** @brief Primitive types. */
    enum PrimType
    {
        PRIM_POLY = 0x20, /** Polygon (`POLY`). */
        PRIM_LINE = 0x40, /** Line (`LINE`). */
        PRIM_RECT = 0x60  /** Rectangle (`TILE` or `SPRT`). */
    };

    /** @brief Primitive rectangle flags. */
    enum PrimRectFlags
    {
        RECT_SIZE_16  = (1 << 3) | (1 << 4), /** Rectangle is 16x16 (`TILE_16` or `SPRT_16`). */
        RECT_SIZE_8   = 1 << 4,              /** Rectangle is 8x8 (`TILE_8` or `SPRT_8`). */
        RECT_SIZE_1   = 1 << 3,              /** Rectangle is 1x1 (`TILE_1`). */
        RECT_TEXTURE  = 1 << 2,              /** Rectangle is textured (`SPRT`). */
        RECT_BLEND    = 1 << 1,              /** Semi-transparency flag. */
        RECT_MODULATE = 1 << 0               /** Use primitive color to modulate texture. */
    };

// @todo Clashes with Psy-Z types. Should that lib be integrated or only have things picked from it?
#if 0
    /** @brief PsyQ matrix. */
    struct MATRIX
    {
        short m[3][3]; /** 3x3 rotation matrix. */
        int   t[3];    /** Transfer vector. */

        MATRIX() = default;
        MATRIX(const Matrix& mat);

        Matrix ToMatrix() const;
    };

    /** @brief PsyQ color vector. */
    struct CVECTOR
    {
        uchar r;
        uchar g;
        uchar b;
        uchar cd;
    };

    /** @brief PsyQ `short`-based XY vector. */
    struct DVECTOR
    {
        union 
        {
            struct
            {
                short x;
                short y;
            };
            struct
            {
                short vx;
                short vy;
            };
        };

        DVECTOR() : x(0), y(0) {}
        DVECTOR(int x, int y) : x(x), y(y) {}
    };

    /** @brief PsyQ coordinate. */
    struct GsCOORD2PARAM
    {
        VECTOR3 scale;
        SVECTOR rotate;
        VECTOR3 trans;
    };

    /** @brief PsyQ bone coordinate. */
    struct GsCOORDINATE2
    {
        ulong                 flg;   /** `bool`. */
        MATRIX                coord;
        MATRIX                workm;
        GsCOORD2PARAM*        param;
        struct GsCOORDINATE2* super; /** Parent. */
        struct GsCOORDINATE2* sub;   /** Child. */
    };

    /** @brief PsyQ rectangle. */
    struct RECT
    {
        short x, y; /* Offset point on VRAM. */
        short w, h; /* Width and height. */
    };

    /** @brief PsyQ Free-size tile. */
    struct TILE
    {
        ulong tag;
        uchar r0, g0, b0, code;
        short x0, y0;
        short w, h;
    };

    /** #brief PsyQ gouraud triangle. */
    struct POLY_G3
    {
        ulong tag;
        uchar r0, g0, b0, code;
        short x0, y0;
        uchar r1, g1, b1, pad1;
        short x1, y1;
        uchar r2, g2, b2, pad2;
        short x2, y2;
    };

    /** #brief PsyQ gouraud quad. */
    struct POLY_G4
    {
        ulong tag;
        uchar r0, g0, b0, code;
        short x0, y0;
        uchar r1, g1, b1, pad1;
        short x1, y1;
        uchar r2, g2, b2, pad2;
        short x2, y2;
        uchar r3, g3, b3, pad3;
        short x3, y3;
    };

    /** @brief PsyQ display area. */
    struct DISPENV
    {
        RECT  disp;       /* Display area. */
        RECT  screen;     /* Display start point. */
        uchar isinter;    /* Rnterlace 0: off 1: on. */
        uchar isrgb24;    /* RGB24 bit mode. */
        uchar pad0, pad1; /* Reserved. */
    };

    /** @brief PsyQ drawing mode. */
    struct DR_MODE
    {
        ulong tag;
        ulong code[2];
    };
#endif

    /** @brief `DVECTOR` variant with a `vz` component instead of `vy`. */
    struct DVECTOR_XZ
    {
        short vx;
        short vz;

        DVECTOR_XZ() : vx(0), vz(0) {}
        DVECTOR_XZ(int x, int z) : vx(x), vz(z) {}
    };

    /** @brief PsyQ `short`-based XYZ vector. */
    struct SVECTOR3
    {
        short vx;
        short vy;
        short vz;

        SVECTOR3() : vx(0), vy(0), vz(0) {}
        SVECTOR3(int x, int y, int z) : vx(x), vy(y), vz(z) {}
    };

    /** @brief PsyQ `int`-based XYZ vector. */
    struct VECTOR3
    {
        int vx;
        int vy;
        int vz;

        VECTOR3() : vx(0), vy(0), vz(0) {}
        VECTOR3(int x, int y, int z) : vx(x), vy(y), vz(z) {}

        Vector3 ToVector3(float qScale) const;
    };

    struct s_Line2d
    {
        Vector2i vertex0;
        Vector2i vertex1;
    };

    struct s_Triangle2d
    {
        Vector2i vertex0;
        Vector2i vertex1;
        Vector2i vertex2;
    };

    struct s_Quad2d
    {
        Vector2i vertex0;
        Vector2i vertex1;
        Vector2i vertex2;
        Vector2i vertex3;
    };

    struct s_ColoredLine2d
    {
        s_Line2d line;
        ushort   r;
        ushort   g;
        ushort   b;
        ushort   __pad_E; // Maybe 4th component of a RGB+code struct?
    };

    struct s_LineBorder
    {
        s_Line2d lines_0[RECT_VERTEX_COUNT];
    };

    struct s_QuadBorder
    {
        s_Quad2d quads_0[RECT_VERTEX_COUNT];
    };

    struct s_PrimColor
    {
        uchar r;
        uchar g;
        uchar b;
        uchar p;
    };
}
