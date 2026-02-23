#pragma once

#include "Math/Objects/Vector3.h"

namespace Silent::Math
{
    /** @brief Infinite 2D plane. */
    class Plane
    {
    public:
        // =======
        // Fields
        // =======

        Vector3 Normal   = Vector3::One;
        float   Distance = 0.0f;

        // =============
        // Constructors
        // =============

        /** @brief Creates a default instance. */
        constexpr Plane() = default;

        /** @brief Constructs an instance from a normal and distance.
         *
         * @param normal Plane normal.
         * @param dist Plane distance.
         */
        constexpr Plane(const Vector3& normal, float dist) :
            Normal(normal),
            Distance(dist) {};

        // ==========
        // Operators
        // ==========

        bool   operator==(const Plane& plane) const;
        bool   operator!=(const Plane& plane) const;
        Plane& operator=(const Plane& plane) = default;
    };
}
