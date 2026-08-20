#pragma once

#include "Utils/Constants.hlsli"

namespace Math
{
    static const float  PI          = 3.1415927f;
    static const float3 LUMA_BT601  = float3(0.299f,  0.587f,  0.114f);
    static const float3 LUMA_BT709  = float3(0.2126f, 0.7152f, 0.0722f);

    /** @brief Remaps a value from one value range to another.
     *
     * @param x Value to remap.
     * @param fromMin Minimim value of first range.
     * @param fromMax Maximum value of first range.
     * @param toMin Minimum value of second range.
     * @param toMax Maximum value of second range.
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

    /** @brief Computes the luma of a color according to BT.601 SD color luma coefficients.
     *
     * @param color Color to evaluate.
     * @return Luma.
     */
    float Luma601(float3 color)
    {
        return dot(color, LUMA_BT601);
    }

    /** @brief Computes the luma of a color according to BT.709 HD color luma coefficients.
     *
     * @param color Color to evaluate.
     * @return Luma.
     */
    float Luma709(float3 color)
    {
        return dot(color, LUMA_BT709);
    }
}
