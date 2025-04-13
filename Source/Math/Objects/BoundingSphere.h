#pragma once

namespace Silent::Math
{
    class      AxisAlignedBoundingBox;
    class      OrientedBoundingBox;
    enum class ContainmentType;

    class BoundingSphere
    {
    public:
        // Fields

        glm::vec3 Center = glm::vec3();
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

        ContainmentType Contains(const glm::vec3& point) const;
        ContainmentType Contains(const BoundingSphere& sphere) const;
        ContainmentType Contains(const AxisAlignedBoundingBox& aabb) const;
        ContainmentType Contains(const OrientedBoundingBox& obb) const;

        // Operators

        bool            operator ==(const BoundingSphere& sphere) const;
        bool            operator !=(const BoundingSphere& sphere) const;
        BoundingSphere& operator =(const BoundingSphere& sphere) = default;
    };
}
