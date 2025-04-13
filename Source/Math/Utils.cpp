#include "Framework.h"

namespace Silent::Math
{
    bool ApproxEqual(float value0, float value1, float epsilon)
    {
        return (std::max(value0, value1) - std::min(value0, value1)) <= epsilon;
    }

    float FloorToStep(float value, float step)
    {
        return floor(value / step) * step;
    }

    float CeilToStep(float value, float step)
    {
        return ceil(value / step) * step;
    }

    float RoundToStep(float value, float step)
    {
        return round(value / step) * step;
    }

    float Remap(float value, float min0, float max0, float min1, float max1)
    {
        float alpha = (value - min0) / (max0 - min0);
        return std::lerp(min1, max1, alpha);
    }
}
