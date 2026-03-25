#pragma once

extern "C" 
{
    #include <psyz.h>

    #include <libgte.h>
    #include <libgpu.h>
    #include <libgs.h>

    #include <libetc.h>
    #include <libsnd.h>
    #include <libspu.h>
}

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
