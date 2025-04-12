#pragma once

namespace Silent::Math
{
    class AxisAlignedBoundingBox;
    class BoundingSphere;
    class OrientedBoundingBox;

    class Ray
    {
    public:
        // Fields

        glm::vec3 Position  = glm::vec3();
        glm::vec3 Direction = glm::vec3(0.0f, 0.0f, 1.0f);

        // Constructors

        constexpr Ray() = default;
        constexpr Ray(const glm::vec3& pos, const glm::vec3& dir) : Position(pos), Direction(dir) {}

        // Inquirers

        std::optional<float> Intersects(const BoundingSphere& sphere) const;
        std::optional<float> Intersects(const AxisAlignedBoundingBox& aabb) const;
        std::optional<float> Intersects(const OrientedBoundingBox& obb) const;
        
        // Operators

        bool operator ==(const Ray& ray) const;
        bool operator !=(const Ray& ray) const;
        Ray& operator =(const Ray& ray) = default;
    };
}
