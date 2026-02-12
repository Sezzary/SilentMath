#pragma once

#include "Math/Objects/Vector3.h"

namespace Silent::Math
{
    class AxisAlignedBoundingBox;
    class BoundingSphere;
    class OrientedBoundingBox;

    /** @brief 3D ray. */
    class Ray
    {
    public:
        // =======
        // Fields
        // =======

        Vector3 Origin    = Vector3::Zero;
        Vector3 Direction = Vector3::UnitZ;

        // =============
        // Constructors
        // =============

        /** @brief Creates a default instance. */
        constexpr Ray() = default;

        /** @brief Creates an instance from an origin and a normalized direction vector.
         *
         * @param origin Origin.
         * @param dir Normalized direction vector.
         */
        constexpr Ray(const Vector3& origin, const Vector3& dir) :
            Origin(origin),
            Direction(dir) {}

        // ==========
        // Inquirers
        // ==========

        /** @brief Checks if a sphere intersects the ray.
         *
         * @param sphere Sphere to test against.
         * @return `true` if the intersection is valid, `false` otherwise.
         */
        std::optional<float> Intersects(const BoundingSphere& sphere) const;

        /** @brief Checks if an AABB intersects the ray.
         *
         * @param aabb AABB to test against.
         * @return `true` if the intersection is valid, `false` otherwise.
         */
        std::optional<float> Intersects(const AxisAlignedBoundingBox& aabb) const;

        /** @brief Checks if an OBB intersects the ray.
         *
         * @param obb OBB to test against.
         * @return `true` if the intersection is valid, `false` otherwise.
         */
        std::optional<float> Intersects(const OrientedBoundingBox& obb) const;

        // ==========
        // Operators
        // ==========

        bool operator==(const Ray& ray) const;
        bool operator!=(const Ray& ray) const;
        Ray& operator=(const Ray& ray) = default;
    };
}
