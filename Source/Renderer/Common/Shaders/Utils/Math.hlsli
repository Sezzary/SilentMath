#pragma once

#include "Utils/Constants.hlsli"

namespace Math
{
    static const float PI = 3.1415927f;

    /** @brief Remaps `x` from one value range to another.
     *
     * @param x Value to remap.
     * @param fromMin Lower bound of the initial range.
     * @param fromMax Upper bound of the initial range.
     * @param toMin Lower bound of the target range.
     * @param toMax Upper bound of the target range.
     * return `x` remapped to the target range.
     */
    float Remap(float x, float fromMin, float fromMax, float toMin, float toMax)
    {
        return toMin + (((x - fromMin) * (toMax - toMin)) / (fromMax - fromMin));
    }

    /** @brief Decodes a color index encoded in the byte-based red channel of a color.
     *
     * @param color Color with an index encoded in the red channel.
     * @return Color index.
     */
    int DecodeColorIdx(float3 color)
    {
        return int((color.r * Constants::UINT8_MAX) + 0.5f);
    }
}
