#pragma once
#include <span>

namespace Silent::Math
{
    class      BoundingSphere;
    class      OrientedBoundingBox;
    enum class ContainmentType;

    class AxisAlignedBoundingBox
    {
    public:
        // Constants

        static constexpr unsigned int CORNER_COUNT = 8;

        // Fields

        glm::vec3 Center  = glm::vec3();
        glm::vec3 Extents = glm::vec3();

        // Constructors
    
        constexpr AxisAlignedBoundingBox() = default;
        constexpr AxisAlignedBoundingBox(const glm::vec3& center, const glm::vec3& extents) : Center(center), Extents(extents) {}
                  AxisAlignedBoundingBox(const std::span<glm::vec3>& points);
                  AxisAlignedBoundingBox(const BoundingSphere& sphere);
                  AxisAlignedBoundingBox(const OrientedBoundingBox& obb);

        // Getters

        float                  GetSurfaceArea() const;
        float                  GetVolume() const;
        std::vector<glm::vec3> GetCorners() const;
        glm::vec3              GetMin() const;
        glm::vec3              GetMax() const;

        // Inquirers

        bool Intersects(const glm::vec3& point) const;
        bool Intersects(const BoundingSphere& sphere) const;
        bool Intersects(const AxisAlignedBoundingBox& aabb) const;
        bool Intersects(const OrientedBoundingBox& obb) const;

        ContainmentType Contains(const glm::vec3& point) const;
        ContainmentType Contains(const BoundingSphere& sphere) const;
        ContainmentType Contains(const AxisAlignedBoundingBox& aabb) const;
        ContainmentType Contains(const OrientedBoundingBox& obb) const;

        // Utilities

        static AxisAlignedBoundingBox Merge(const AxisAlignedBoundingBox& aabb0, const AxisAlignedBoundingBox& aabb1);
        void                          Merge(const AxisAlignedBoundingBox& aabb);

        // Operators

        bool                    operator ==(const AxisAlignedBoundingBox& aabb) const;
        bool                    operator !=(const AxisAlignedBoundingBox& aabb) const;
        AxisAlignedBoundingBox& operator =(const AxisAlignedBoundingBox& aabb) = default;
    };
}
