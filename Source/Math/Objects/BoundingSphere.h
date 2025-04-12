#pragma once

namespace Silent::Math
{
    class AxisAlignedBoundingBox;
    class OrientedBoundingBox;

    class BoundingSphere
    {
    public:
        // Fields

        glm::vec3 Center = {};
        float     Radius = 0.0f;

        // Constructors

        constexpr BoundingSphere() = default;
        constexpr BoundingSphere(const glm::vec3& center, float radius) : Center(center), Radius(radius) {}

        // Getters

        float GetSurfaceArea() const;
        float GetVolume() const;

        // Inquirers

        bool Intersects(const glm::vec3& point) const;
        bool Intersects(const BoundingSphere& sphere) const;
        bool Intersects(const AxisAlignedBoundingBox& aabb) const;
        bool Intersects(const OrientedBoundingBox& obb) const;
    };
}
