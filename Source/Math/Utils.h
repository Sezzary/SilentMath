#pragma once

#include "Math/Constants.h"

namespace Silent::Math
{
    bool  ApproxEqual(float value0, float value1, float epsilon = EPSILON);
    float FloorToStep(float value, float step);
    float CeilToStep(float value, float step);
    float RoundToStep(float value, float step);
    float Remap(float value, float min0, float max0, float min1, float max1);
}
