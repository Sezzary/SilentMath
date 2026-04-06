#pragma once

#include "Math/Constants.h"
#include "Math/Objects/Vector3.h"

namespace Silent::Math
{
    /** @brief Floors a value to the nearest step.
     *
     * @param x Value to floor.
     * @param step Flooring step.
     * @return `x` floored to the nearest `step`.
     */
    float FloorToStep(float x, float step);

    /** @brief Ceils a value to the nearest step.
     *
     * @param x Value to ceil.
     * @param step Ceiling step.
     * @return `x` Ceiled to the nearest `step`.
     */
    float CeilToStep(float x, float step);

    /** @brief Rounds a value to the nearest step.
     *
     * @param x Value to round.
     * @param step Rounding step.
     * @return `x` rounded to the nearest `step`.
     */
    float RoundToStep(float x, float step);

    /** @brief Remaps a value from one value range to another.
     *
     * @param x Value to remap.
     * @param min0 Minimim value of first range.
     * @param max0 Maximum value of first range.
     * @param min1 Minimum value of second range.
     * @param max1 Maximum value of second range.
     * @return `x` remapped from the first range to the second.
     */
    float Remap(float x, float min0, float max0, float min1, float max1);

    /** @brief Checks if two values are approximately equal.
     *
     * @param a First value.
     * @param b Second value.
     * @param epsilon Comparison epsilon.
     * @return `true` if the values are approximately equal, `false` otherwise.
     */
    bool IsApproxEqual(float a, float b, float epsilon = EPSILON);

    /** @brief Computes the surface slope angle, relative to a downward axis.
     *
     * @param normal Surface normal.
     * @param axis Downward axis.
     * @return Surface slope angle.
     */
    short GetSurfaceSlopeAngle(const Vector3& normal, const Vector3& axis = Vector3::UnitY);

    /** @brief Computes the surface aspect angle, relative to a downward axis.
     *
     * @param normal Surface normal.
     * @param axis Downward axis.
     * @return Surface aspect angle.
     */
    short GetSurfaceAspectAngle(const Vector3& normal, const Vector3& axis = Vector3::UnitY);

    /** @brief Computes the distance to a line.
     *
     * @param from Start.
     * @param lineStart Line start.
     * @param lineEnd Line end.
     * @return Distance to the line.
     */
    float GetDistanceToLine(const Vector3& from, const Vector3& lineStart, const Vector3& lineEnd);

    /** @brief Gets the closest point on a line.
     *
     * @param from Start.
     * @param lineStart Line start.
     * @param lineEnd Line end.
     * @return Closest point on the line.
     */
    Vector3 GetClosestPointOnLine(const Vector3& from, const Vector3& lineStart, const Vector3& lineEnd);

    /** @brief Gets the closest point on a line, perpendicular to a downward axis.
     *
     * @param from Start.
     * @param lineStart Line start.
     * @param lineEnd Line end.
     * @param axis Downward axis.
     * @return Closest point on the line.
     */
    Vector3 GetClosestPointOnLinePerp(const Vector3& from, const Vector3& lineStart, const Vector3& lineEnd,
                                      const Vector3& axis = Vector3::UnitY);

    /** @brief Checks if a given point is in front of another, relative to a downward axis.
     *
     * @param from First point.
     * @param to Second point.
     * @param ref Reference orienting point.
     * @param axis Downward axis.
     * @return `true` if the second point is in front of the first, `false` otherwise.
     */
    bool IsPointInFront(const Vector3& from, const Vector3& to, const Vector3& ref,
                        const Vector3& axis = Vector3::UnitY);

    /** @brief Checks if a given point is in to the left of another, relative to a downward axis.
     *
     * @param from First point.
     * @param to Second point.
     * @param ref Reference orienting point.
     * @param axis Downward axis.
     * @return `true` if the second point is to the left of the first, `false` otherwise.
     */
    bool IsPointOnLeft(const Vector3& from, const Vector3& to, const Vector3& ref,
                       const Vector3& axis = Vector3::UnitY);
}
