#pragma once

#include "Math/Constants.h"

namespace Silent::Math
{
    bool  IsApproxEqual(float a, float b, float epsilon = EPSILON);
    float FloorToStep(float x, float step);
    float CeilToStep(float x, float step);
    float RoundToStep(float x, float step);
    float Remap(float x, float min0, float max0, float min1, float max1);
}
