#pragma once

#include "Framework.h"

namespace Silent::Math
{
    using FpAngle = short;

    constexpr uint Q4_SHIFT       = 4;                                         /** Used for: Q27.4 positions. */
    constexpr uint Q8_SHIFT       = 8;                                         /** Used for: Q8.8 range limits. Q24.8 tile units. */
    constexpr uint Q12_SHIFT      = 12;                                        /** Used for: Q3.12 alphas. Q19.12 timers, trigonometry. */
    constexpr uint FP_ANGLE_COUNT = (std::numeric_limits<FpAngle>::max() -
                                     std::numeric_limits<FpAngle>::min()) + 1; /** Number of fixed-point angles in Q1.15 format. */

    constexpr float PI       = glm::pi<float>();
    constexpr float PI_MUL_4 = PI * 4.0f;
    constexpr float PI_MUL_2 = PI * 2.0f;
    constexpr float PI_DIV_2 = PI / 2.0f;
    constexpr float PI_DIV_4 = PI / 4.0f;
    constexpr float SQRT_2   = 1.41421356237f;
    constexpr float EPSILON  = 0.00001f;

    enum class ContainmentType
    {
        Disjoint,
        Intersects,
        Contains
    };

    /** @brief Squares a numeric value. */
    constexpr auto SQUARE = [](auto x)
    {
        return x * x;
    };

    /** @brief Cubes a numeric value. */
    constexpr auto CUBE = [](auto x)
    {
        return x * x * x;
    };

    /** @brief Rounds a floating-point value. */
    constexpr float ROUND(float x)
    {
        return (float)((x > 0.0f) ? (int)(x + 0.5f) : (int)(x - 0.5f));
    }
    
    /** @brief Converts an integer to a fixed-point Q format. */
    constexpr int FP_TO(int x, uint shift)
    {
        return x << shift;
    }

    /** @brief Converts a float to a fixed-point Q format. */
    constexpr int FP_FLOAT_TO(float x, uint shift)
    {
        return (int)ROUND(x * FP_TO(1, shift));
    }

    /** @brief Converts an integer from a fixed-point Q format. */
    constexpr int FP_FROM(int x, uint shift)
    {
        return x >> shift;
    }

    /** @brief Multiplies two integers in a fixed-point Q format and converts the result from the fixed-point Q format. */
    constexpr int FP_MULTIPLY(int a, int b, uint shift)
    {
        return (a * b) >> shift;
    }

    /** @brief Multiplies an integer by a float converted to fixed-point Q format and converts the result from the fixed-point Q format. */
    constexpr int FP_MULTIPLY_FLOAT(int a, float b, uint shift)
    {
        return FP_MULTIPLY(a, FP_FLOAT_TO(b, shift), shift);
    }

    /** @brief Multiplies an integer by a float converted to fixed-point Q format, using 64-bit intermediate for higher precision. */
    constexpr int FP_MULTIPLY_PRECISE(int a, float b, uint shift)
    {
        return FP_MULTIPLY((int64)a, FP_FLOAT_TO(b, shift), shift);
    }

    /** @brief Converts a floating-point alpha in the range [0.0f, 1.0f] to a fixed-point alpha in Q3.12 format. */
    constexpr short FP_ALPHA(float alpha)
    {
        return (short)FP_FLOAT_TO(alpha, Q12_SHIFT);
    }

    /** @brief Converts a normalized color value in the range [0.0f, 1.0f] to an 8-bit color format in the range [0, 255]. */
    constexpr uchar FP_COLOR(float val)
    {
        return (uchar)(val * 255);
    }

    /** @brief Converts floating-point degrees to fixed-point angles in Q1.15 format. */
    constexpr FpAngle FP_ANGLE(float deg)
    {
        return (FpAngle)ROUND(deg * (FP_ANGLE_COUNT / 360.0f));
    }

    /** @brief Converts floating-point radians to fixed-point angles in Q1.15 format. */
    constexpr FpAngle FP_ANGLE_FROM_RAD(float rad)
    {
        return FP_ANGLE(rad / (PI / 180.0f));
    }

    /** @brief Converts fixed-point angles in Q1.15 format to floating-point radians. */
    constexpr float FP_ANGLE_TO_RAD(FpAngle angle)
    {
        return (angle * ((float)FP_ANGLE_COUNT / 360.0f)) * (PI / 180.0f);
    }

    /** @brief Converts floating-point meters to fixed-point world units in Q12.8 format. */
    constexpr int FP_METER(float meters)
    {
        return FP_FLOAT_TO(meters, Q8_SHIFT);
    }
}
