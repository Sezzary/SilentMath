#pragma once

#include "Utils/Constants.hlsli"

namespace Math
{
    static const float PI = 3.1415927f;

    /** @brief Remaps a value from one value range to another.
     *
     * @param x Value to remap.
     * @param min0 Minimim value of first range.
     * @param max0 Maximum value of first range.
     * @param min1 Minimum value of second range.
     * @param max1 Maximum value of second range.
     * @return `x` remapped from the first range to the second.
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
