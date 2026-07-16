#pragma once

namespace Math
{
    static const float3 LUMA_BT601 = float3(0.299f, 0.587f, 0.114f);

    float Remap(float x, float fromMin, float fromMax, float toMin, float toMax)
    {
        return toMin + (((x - fromMin) * (toMax - toMin)) / (fromMax - fromMin));
    }
}
