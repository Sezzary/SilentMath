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

    /** @brief World position and rotation. */
    struct s_Pose
    {
        VECTOR3  position; /** Q19.12 */
        SVECTOR3 rotation; /** Q19.12 */
    };

    /** @brief Polygon normal. */
    typedef struct _Normal
    {
        s8 nx;
        s8 ny;
        s8 nz;
        u8 count;
    } s_Normal;

    /** @brief 2D screen-space line. */
    struct s_Line2d
    {
        Vector2i vertex0;
        Vector2i vertex1;
    };

    /** @brief 2D screen-space triangle. */
    struct s_Triangle2d
    {
        Vector2i vertex0;
        Vector2i vertex1;
        Vector2i vertex2;
    };

    /** @brief 2D screen-space quad. */
    struct s_Quad2d
    {
        Vector2i vertex0;
        Vector2i vertex1;
        Vector2i vertex2;
        Vector2i vertex3;
    };

    /** @brief Colored 2D screen-space line. */
    struct s_ColoredLine2d
    {
        s_Line2d line;
        ushort   r;
        ushort   g;
        ushort   b;
    };

    /** @brief 2D screen-space line border. */
    struct s_LineBorder
    {
        s_Line2d lines[RECT_VERTEX_COUNT];
    };

    /** @brief 2D screen-space quad border. */
    struct s_QuadBorder
    {
        s_Quad2d quads[RECT_VERTEX_COUNT];
    };

    /** @brief Primitive color. */
    struct s_PrimColor
    {
        uchar r;
        uchar g;
        uchar b;
        uchar p;
    };
}
