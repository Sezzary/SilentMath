#ifndef COMMON_UTILS_HLSLI
#define COMMON_UTILS_HLSLI

namespace Utils
{
    float Remap(float val, float fromMin, float fromMax, float toMin, float toMax)
    {
        return toMin + (((val - fromMin) * (toMax - toMin)) / (fromMax - fromMin));
    }
}

#endif
