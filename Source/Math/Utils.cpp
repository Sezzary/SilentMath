#include "Framework.h"

namespace Silent::Math
{
    bool IsApproxEqual(float a, float b, float epsilon)
    {
        return (std::max(a, b) - std::min(a, b)) <= epsilon;
    }

    float FloorToStep(float x, float step)
    {
        return floor(x / step) * step;
    }

    float CeilToStep(float x, float step)
    {
        return ceil(x / step) * step;
    }

    float RoundToStep(float x, float step)
    {
        return round(x / step) * step;
    }

    float Remap(float x, float min0, float max0, float min1, float max1)
    {
        float alpha = (x - min0) / (max0 - min0);
        return std::lerp(min1, max1, alpha);
    }
}
