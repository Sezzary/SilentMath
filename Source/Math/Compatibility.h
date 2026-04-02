#pragma once

#include "Math/Objects/EulerAngles.h"
#include "Math/Objects/Matrix.h"
#include "Math/Objects/Vector2i.h"
#include "Math/Objects/Vector3i.h"

namespace Silent::Math
{
    constexpr int RECT_VERTEX_COUNT = 4;
    constexpr int BOX_VERTEX_COUNT  = 8;

    /** @brief SH `DVECTOR` variant with a `vz` component instead of `vy`. */
    struct DVECTOR_XZ
    {
        short vx;
        short vz;

        DVECTOR_XZ() : vx(0), vz(0) {}
        DVECTOR_XZ(int x, int z) : vx(x), vz(z) {}
    };

    /** @brief SH `SVECTOR` variant without padding. */
    struct SVECTOR3
    {
        short vx;
        short vy;
        short vz;

        SVECTOR3() : vx(0), vy(0), vz(0) {}
        SVECTOR3(int x, int y, int z) : vx(x), vy(y), vz(z) {}
    };

    /** @brief SH `VECTOR` variant without padding. */
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
