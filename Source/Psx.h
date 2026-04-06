#pragma once

#include "Types.h"

extern "C" 
{
    #include <psyz.h>

    #include <libgte.h>
    #include <libgpu.h>
    #include <libgs.h>

    #include <libcd.h>
    #include <libetc.h>
    #include <libsnd.h>
    #include <libspu.h>
}

using namespace Silent;

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

/** @brief LBA Integer to BCD.
 *
 * @param i LBA integer.
 * @return BCD.
 */
constexpr int itob(int i)
{
    return ((i / 10) * 16) + (i % 10);
}

/** @brief Computes the square root of a Q19.12 value.
 *
 * @param x Value to square.
 * @return `x` squared.
 */
q19_12 Compat_SquareRoot12(q19_12 x);

/** @brief Computes a negated rotation matrix from ZXY Euler angles.
 *
 * @param rot Q3.12 Euler rotation.
 * @param mat Output rotation matrix.
 */
void Math_RotMatrixZxyNeg(SVECTOR* rot, MATRIX* mat);

/** @brief Computes a negated rotation matrix from ZXY Euler angles.
 * Wraps `Math_RotMatrixZxyNeg`.
 *
 * @param rot Q3.12 Euler rotation.
 * @param mat Output rotation matrix.
 */
void Math_RotMatrixZxyNegGte(SVECTOR* rot, MATRIX* mat);

/** @brief Computes a rotation matrix from XYZ Euler angles.
 *
 * @param rot Q3.12 Euler rotation.
 * @param mat Output rotation matrix.
 */
void Math_RotMatrixXyz(SVECTOR* rot, MATRIX* mat);

/** @brief Computes a rotation matrix from ZXY Euler angles.
 *
 * @param rot Q3.12 Euler rotation.
 * @param mat Output rotation matrix.
 */
void Math_RotMatrixZxy(SVECTOR* rot, MATRIX* mat);

/** @brief Computes a rotation matrix from XYX Euler angles.
 *
 * @param rot Q3.12 Euler rotation.
 * @param mat Output rotation matrix.
 */
void Math_RotMatrixXyxGte(SVECTOR* rot, MATRIX* mat);

/** @brief Computes a rotation matrix for Z-axis rotation angle.
 *
 * @param angle Z-axis angle.
 * @param mat Output rotation matrix.
 */
MATRIX* Math_RotMatrixZ(q19_12 angle, MATRIX* mat);

void Math_MatrixTransform(MATRIX* mat, VECTOR3* in, VECTOR3* out);
