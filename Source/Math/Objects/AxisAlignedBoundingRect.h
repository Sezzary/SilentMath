#pragma once

#include "Math/Objects/Vector2.h"

namespace Silent::Math
{
    /** @brief 2D axis-aligned bounding rectangle (AABR). */
    class AxisAlignedBoundingRect
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

        /** @brief Creates a default instance. */
        constexpr AxisAlignedBoundingRect() = default;

        /** @brief Creates an instance from a center and extents.
         *
         * @param center Rectangle center.
         * @param extents Rectangle extents.
         */
        constexpr AxisAlignedBoundingRect(const Vector2& center, const Vector2& extents) :
            Center(center),
            Extents(extents) {}

        // ==========
        // Inquirers
        // ==========

        /** @brief Checks if a point intersects the AABR.
         *
         * @param point Point to test against.
         * @return `true` if the intersection is valid, `false` otherwise.
         */
        bool Intersects(const Vector2& point) const;
    };
}
