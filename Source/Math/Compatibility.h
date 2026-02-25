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

// @todo Clashes with Psy-Z types. Should that lib be integrated or only have things picked from it?
//#if 0
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

    /** @brief `DVECTOR` variant with a `vz` component instead of `vy`. */
    struct DVECTOR_XZ
    {
        union 
        {
            struct
            {
                short x;
                short z;
            };
            struct
            {
                short vx;
                short vz;
            };
        };

        DVECTOR_XZ() : x(0), z(0) {}
        DVECTOR_XZ(int x, int z) : x(x), z(z) {}
    };

    /** @brief PsyQ `short`-based XYZ vector. */
    struct SVECTOR3
    {
        union 
        {
            struct
            {
                short x;
                short y;
                short z;
            };
            struct
            {
                short vx;
                short vy;
                short vz;
            };
        };

        SVECTOR3() : x(0), y(0), z(0) {}
        SVECTOR3(int x, int y, int z) : x(x), y(y), z(z) {}
    };
    using SVECTOR = SVECTOR3;

    /** @brief PsyQ `int`-based XYZ vector. */
    struct VECTOR3
    {
        union 
        {
            struct
            {
                short x;
                short y;
                short z;
            };
            struct
            {
                short vx;
                short vy;
                short vz;
            };
        };

        VECTOR3() : x(0), y(0), z(0) {}
        VECTOR3(int x, int y, int z) : x(x), y(y), z(z) {}

        Vector3 ToVector3(float qScale) const;
    };
    using VECTOR = VECTOR3;

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

//#endif

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
