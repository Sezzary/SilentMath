#include "Framework.h"
#include "Math/Rng.h"

namespace Silent::Math
{
    /** @brief Current random seed for the `Rng_Rand32` function.
     * The value is updated with each call to the RNG functions.
     */
    static uint RngSeed = 0;

    uint Rng_Rand32()
    {
        RngSeed = (RngSeed * 1664525) + 1013904223;
        return RngSeed;
    }

    uint Rng_Rand16()
    {
        return Rng_Rand32() >> 17;
    }

    uint Rng_GetSeed()
    {
        return RngSeed;
    }

    void Rng_SetSeed(uint newSeed)
    {
        RngSeed = newSeed;
    }

    ushort Rng_Rand12()
    {
        return Rng_Rand32() >> 20;
    }

    bool TestRng(uint bits)
    {
        return Rng_Rand16() & ((1 << bits) - 1);
    }
}
