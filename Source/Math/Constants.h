#pragma once

namespace Silent::Math
{
    constexpr float PI       = glm::pi<float>();
    constexpr float PI_MUL_4 = PI * 4.0f;
    constexpr float PI_MUL_2 = PI * 2.0f;
    constexpr float PI_DIV_2 = PI / 2.0f;
    constexpr float PI_DIV_4 = PI / 4.0f;
    constexpr float RADIAN   = PI / 180.0f;
    constexpr float SQRT_2   = 1.41421356237f;
    constexpr float EPSILON  = 0.00001f;

    constexpr unsigned int Q4_SHIFT       = 4;     /** Used for: Q27.4 positions. */
    constexpr unsigned int Q8_SHIFT       = 8;     /** Used for: Q8.8 range limits. Q24.8 tile units. */
    constexpr unsigned int Q12_SHIFT      = 12;    /** Used for: Q3.12 alphas. Q19.12 timers, trigonometry. */
    constexpr unsigned int SIN_LUT_SIZE   = 4096;  /** Number of entries in the sine lookup table. */
    constexpr unsigned int FP_ANGLE_COUNT = 65536; /** Number of fixed-point angles in Q1.15 format. */

    constexpr auto SQUARE = [](auto x) { return (x * x); };
    constexpr auto CUBE   = [](auto x) { return (x * x * x); };

    enum class ContainmentType
    {
        Disjoint,
        Intersects,
        Contains
    };

    constexpr float ROUND(float value)
    {
        return ((value > 0.0f) ? int(value + 0.5f) : int(value - 0.5f));
    }
    
    /** Converts an integer to a fixed-point Q format. */
    constexpr int FP_TO(int val, unsigned int shift)
    {
        return val << shift;
    }

    /** Converts a float to a fixed-point Q format. */
    constexpr float FP_FLOAT_TO(float val, unsigned int shift)
    {
        return val * FP_TO(1, shift);
    }

    /** Converts an integer from a fixed-point Q format. */
    constexpr int FP_FROM(int val, unsigned int shift)
    {
        return val >> shift;
    }

    /** Multiplies two integers in a fixed-point Q format and converts the result from the fixed-point Q format. */
    constexpr int FP_MULTIPLY(int val0, int val1, unsigned int shift)
    {
        return (val0 * val1) >> shift;
    }

    /** Multiplies an integer by a float converted to fixed-point Q format, using 64-bit intermediate via Math_MulFixed for higher precision. */
    /*#define FP_MULTIPLY_PRECISE(val0, val1, shift) \
        (Math_MulFixed((val0), FP_FLOAT_TO((val1), (shift)), (shift)))*/

    /** Converts a floating-point alpha in the range [0.0f, 1.0f] to a fixed-point alpha in Q3.12 format. */
    constexpr short FP_ALPHA(float alpha)
    {
        return (short)(alpha * (1 << (Q12_SHIFT)));
    }

    /** Converts floating-point degrees to fixed-point angles in Q1.15 format. */
    constexpr short FP_ANGLE(float deg)
    {
        return (short)(deg * (FP_ANGLE_COUNT / 360.0f));
    }

    constexpr short FP_ANGLE_FROM_RAD(float rad)
    {
	    return (short)ROUND((rad / RADIAN) * ((float)FP_ANGLE_COUNT / 360.0f));
    }

    constexpr float FP_ANGLE_TO_RAD(short angle)
    {
        return (angle * ((float)FP_ANGLE_COUNT / 360.0f)) * RADIAN;
    }

    /** Converts floating-point tile units to fixed-point world units in Q12.8 format. */
    constexpr int FP_METER(float meters)
    {
        return (int)(meters * (1 << Q8_SHIFT));
    }
}
