#pragma once

namespace Math
{
    static const uint   UINT8_MAX  = 255;
    static const float3 LUMA_BT601 = float3(0.299f, 0.587f, 0.114f);

    float Remap(float val, float fromMin, float fromMax, float toMin, float toMax)
    {
        return toMin + (((val - fromMin) * (toMax - toMin)) / (fromMax - fromMin));
    }
}
