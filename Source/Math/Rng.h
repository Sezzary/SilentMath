#pragma once

namespace Silent::Math
{
    /** @brief Generates a new random 32-bit unsigned integer and updates
     * `g_RngSeed`.
     *
     * This function implements a Linear Congruential Generator (LCG) Random Number
     * Generator (RNG) algorithm, as outlined in "Numerical Recipes" (Second
     * Edition, Chapter 7.1, An Even Quicker Generator). It produces a pseudo-random
     * 32-bit unsigned integer value.
     *
     * @return A new pseudo-random 32-bit unsigned integer (`uint`).
     */
    uint Rng_Rand32();

    /** @brief Generates a new random 16-bit unsigned integer.
     * 
     * This function calls `Rng_Rand32` to generate a random number, then
     * shifts the result right to produce a value within the range
     * of `[0, 0x7FFF]`.
     *
     * @return A random positive 16-bit unsigned integer as a 32-bit unsigned
     * integer (`uint`).
     */
    uint Rng_Rand16();

    /** @brief Returns the current random seed value.
     *
     * This function retrieves and returns the current value of the global variable
     * `g_RngSeed`.
     *
     * @return The current random seed as a 32-bit unsigned integer (`uint`).
     */
    uint Rng_GetSeed();

    /** @brief Sets the random seed to a specified value.
     *
     * This function updates the global variable `g_RngSeed` with the given
     * seed value.
     *
     * @param newSeed The new seed value to be set, as a 32-bit unsigned integer
     * (`uint`).
     */
    void Rng_SetSeed(uint newSeed);

    /** @brief Generates a new random 12-bit short integer.
     *
     * This function calls `Rng_Rand32` to generate a random number, then
     * shifts the result right to produce a value within the range
     * of `0` to `0xFFF` (12-bit).
     *
     * @return A random 12-bit short integer in the range `[0, 0xFFF]`.
     */
    ushort Rng_Rand12();

    /** @brief Tests if a probability is met.
     *
     * This function evaluates the probability by performing a bitwise AND
     * operation with a mask that has the specified number of bits set to 1.
     *
     * Bits | Mask   | Chance  
     * -----|--------|--------
     * 1    | 0x1    | 50%
     * 2    | 0x3    | 25%
     * 3    | 0x7    | 12.5%
     * 4    | 0xF    | 6.25%
     * 5    | 0x1F   | 3.125%
     * 6    | 0x3F   | 1.563%
     * 7    | 0x7F   | 0.781%
     * 8    | 0xFF   | 0.391%
     * 9    | 0x1FF  | 0.195%
     * 10   | 0x3FF  | 0.098%
     * 11   | 0x7FF  | 0.049%
     * 12   | 0xFFF  | 0.024%
     * 13   | 0x1FFF | 0.012%
     * 14   | 0x3FFF | 0.006%
     * 15   | 0x7FFF | 0.003%
     * 16   | 0xFFFF | 0.002%
     */
    bool TestRng(uint bits);
}
