#pragma once

/** @brief Computes the sine in Q19.12 of degrees in Q19.12, integer range `[0, 4096]`.
 *
 * TODO: Matched on decomp.me.
 * Possible original name: `shRsin`.
 *
 * @param angle Fixed-point degrees in Q19.12, integer range `[0, 4096]`.
 * @return Sine in Q19.12, integer range `[0, 4096]`.
 */
q19_12 Math_Sin(q19_12 angle);

/** @brief Computes the cosine in Q19.12 of degrees in Q19.12, integer range `[0, 4096]`.
 *
 * TODO: Matched on decomp.me.
 * Possible original name: `shRcos`.
 *
 * @param angle Fixed-point degrees in Q19.12, integer range `[0, 4096]`.
 * @return Cosine in Q19.12, integer range `[0, 4096]`.
 */
q19_12 Math_Cos(q19_12 angle);
