#include "Framework.h"
#include "Psx.h"

// @todo Try making a PSX graphics shim.

q19_12 Compat_SquareRoot12(q19_12 x)
{
    if (x <= Q12(0.0f))
    {
        return Q12(0.0f);
    }

    return (q19_12)(sqrt((float)x) * 64.0); // `sqrt(Q12) = sqrt(x / 4096) * 4096 = sqrt(x) * 64`
}

void Math_RotMatrixZxyNeg(SVECTOR* rot, MATRIX* mat)
{
    float rx = (((float)rot->vx / 4096.0f) * 2.0f) * PI;
    float ry = (((float)rot->vy / 4096.0f) * 2.0f) * PI;
    float rz = (((float)rot->vz / 4096.0f) * 2.0f) * PI;

    // Negate angles.
    float sx = sin(-rx), cx = cos(-rx);
    float sy = sin(-ry), cy = cos(-ry);
    float sz = sin(-rz), cz = cos(-rz);

    // ZXY rotation order: `Ry * Rx * Rz`.
    mat->m[0][0] = (short)(( (cy * cz) + (sy * sx * sz)) * 4096.0f);
    mat->m[0][1] = (short)((  cx * sz)                   * 4096.0f);
    mat->m[0][2] = (short)(((-sy * cz) + (cy * sx * sz)) * 4096.0f);

    mat->m[1][0] = (short)(((-cy * sz) + (sy * sx * cz)) * 4096.0f);
    mat->m[1][1] = (short)((  cx * cz)                   * 4096.0f);
    mat->m[1][2] = (short)((( sy * sz) + (cy * sx * cz)) * 4096.0f);

    mat->m[2][0] = (short)(( sy * cx) * 4096.0f);
    mat->m[2][1] = (short)((-sx)      * 4096.0f);
    mat->m[2][2] = (short)(( cy * cx) * 4096.0f);
}

void Math_RotMatrixZxyNegGte(SVECTOR* rot, MATRIX* mat)
{
    Math_RotMatrixZxyNeg(rot, mat);
}

void Math_RotMatrixXyz(SVECTOR* rot, MATRIX* mat)
{
    float rx = ((float)rot->vx / 4096.0f) * 2.0 * M_PI;
    float ry = ((float)rot->vy / 4096.0f) * 2.0 * M_PI;
    float rz = ((float)rot->vz / 4096.0f) * 2.0 * M_PI;

    float sx = sin(rx);
    float cx = cos(rx);
    float sy = sin(ry);
    float cy = cos(ry);
    float sz = sin(rz);
    float cz = cos(rz);

    // XYZ rotation order: `Rz * Ry * Rx`.
    mat->m[0][0] = (short)(( cy * cz) * 4096.0f);
    mat->m[0][1] = (short)((-cy * sz) * 4096.0f);
    mat->m[0][2] = (short)(( sy)      * 4096.0f);

    mat->m[1][0] = (short)((( sx * sy * cz) + (cx * sz)) * 4096.0f);
    mat->m[1][1] = (short)(((-sx * sy * sz) + (cx * cz)) * 4096.0f);
    mat->m[1][2] = (short)(( -sx * cy)                   * 4096.0f);

    mat->m[2][0] = (short)(((-cx * sy * cz) + (sx * sz)) * 4096.0f);
    mat->m[2][1] = (short)((( cx * sy * sz) + (sx * cz)) * 4096.0f);
    mat->m[2][2] = (short)((  cx * cy)                   * 4096.0f);
}

void Math_RotMatrixZxy(SVECTOR* rot, MATRIX* mat)
{
    float radX = (((float)rot->vx / 4096.0f) * 2.0f) * PI;
    float radY = (((float)rot->vy / 4096.0f) * 2.0f) * PI;
    float radZ = (((float)rot->vz / 4096.0f) * 2.0f) * PI;

    float sinX = sin(radX);
    float cosX = cos(radX);
    float sinY = sin(radY);
    float cosY = cos(radY);
    float sinZ = sin(radZ);
    float cosZ = cos(radZ);

    // ZXY rotation order: `Ry * Rx * Rz`/
    mat->m[0][0] = (short)(( (cosY * cosZ) + (sinY * sinX * sinZ)) * 4096.0f);
    mat->m[0][1] = (short)((  cosX * sinZ)                   * 4096.0f);
    mat->m[0][2] = (short)(((-sinY * cosZ) + (cosY * sinX * sinZ)) * 4096.0f);

    mat->m[1][0] = (short)(((-cosY * sinZ) + (sinY * sinX * cosZ)) * 4096.0f);
    mat->m[1][1] = (short)((  cosX * cosZ)                   * 4096.0f);
    mat->m[1][2] = (short)(( (sinY * sinZ) + (cosY * sinX * cosZ)) * 4096.0f);

    mat->m[2][0] = (short)(( sinY * cosX) * 4096.0f);
    mat->m[2][1] = (short)((-sinX)      * 4096.0f);
    mat->m[2][2] = (short)(( cosY * cosX) * 4096.0f);
}

void Math_RotMatrixXyxGte(SVECTOR* rot, MATRIX* mat)
{
    Math_RotMatrixXyz(rot, mat);
}

MATRIX* Math_RotMatrixZ(q19_12 angle, MATRIX* mat)
{
    float rad    = (((float)angle / 4096.0f) * 2.0f) * PI;
    float sinRad = sin(rad);
    float cosRad = cos(rad);

    mat->m[0][0] = (short)( cosRad * 4096.0f);
    mat->m[0][1] = (short)(-sinRad * 4096.0f);
    mat->m[0][2] = 0;

    mat->m[1][0] = (short)(sinRad * 4096.0f);
    mat->m[1][1] = (short)(cosRad * 4096.0f);
    mat->m[1][2] = 0;

    mat->m[2][0] = 0;
    mat->m[2][1] = 0;
    mat->m[2][2] = 4096.0f;

    return mat;
}

void Math_MatrixTransform(MATRIX* mat, VECTOR3* in, VECTOR3* out)
{
    s32 x = in->vx;
    s32 y = in->vy;
    s32 z = in->vz;

    out->vx = ((s32)mat->m[0][0] * x + (s32)mat->m[0][1] * y + (s32)mat->m[0][2] * z) >> 12;
    out->vy = ((s32)mat->m[1][0] * x + (s32)mat->m[1][1] * y + (s32)mat->m[1][2] * z) >> 12;
    out->vz = ((s32)mat->m[2][0] * x + (s32)mat->m[2][1] * y + (s32)mat->m[2][2] * z) >> 12;

    out->vx += mat->t[0];
    out->vy += mat->t[1];
    out->vz += mat->t[2];
}
