#pragma once

namespace Math
{
    static const float PI = 3.1415927f;

    float Remap(float x, float fromMin, float fromMax, float toMin, float toMax)
    {
        return toMin + (((x - fromMin) * (toMax - toMin)) / (fromMax - fromMin));
    }
}
