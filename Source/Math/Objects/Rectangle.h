#pragma once

#include "Math/Objects/Vector2.h"

namespace Silent::Math
{
    /** @brief 2D rectangle. */
    class Rectangle
    {
    public:
        // =======
        // Fields
        // =======

        Vector2 Center  = Vector2::Zero;
        Vector2 Extents = Vector2::One;

        // =============
        // Constructors
        // =============

        /** @brief Constructs a default `Rectangle`. */
        Rectangle() = default;

        // ==========
        // Inquirers
        // ==========

        /** @brief Checks if a point intersects the rectangle.
         *
         * @param point Point to test against.
         * @return `true` if the intersection is valid, `false` otherwise.
         */
        bool Intersects(const Vector2& point) const;
    };
}
