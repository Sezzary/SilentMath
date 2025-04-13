#pragma once

namespace Silent::Math
{
    class      AxisAlignedBoundingBox;
    class      BoundingSphere;
    enum class ContainmentType;

    class OrientedBoundingBox
    {
    public:
        // Constants

        static constexpr unsigned int CORNER_COUNT = 8;

        // Fields

        glm::vec3 Center   = glm::vec3();
        glm::vec3 Extents  = glm::vec3();
        glm::quat Rotation = glm::quat();

        // Constructors

       constexpr OrientedBoundingBox() = default;
       constexpr OrientedBoundingBox(const glm::vec3& center, const glm::vec3& extents, const glm::quat rot) : Center(center), Extents(extents), Rotation(rot) {}

       // Getters

       float                  GetSurfaceArea() const;
       float                  GetVolume() const;
       std::vector<glm::vec3> GetCorners() const;
       glm::mat4              GetTransformMatrix() const;

       // Utilities

       bool Intersects(const glm::vec3& point) const;
       bool Intersects(const BoundingSphere& sphere) const;
       bool Intersects(const AxisAlignedBoundingBox& aabb) const;
       bool Intersects(const OrientedBoundingBox& obb) const;

       ContainmentType Contains(const glm::vec3& point) const;
       ContainmentType Contains(const BoundingSphere& sphere) const;
       ContainmentType Contains(const AxisAlignedBoundingBox& aabb) const;
       ContainmentType Contains(const OrientedBoundingBox& obb) const;

       // Operators

       bool                 operator ==(const OrientedBoundingBox& obb) const;
       bool                 operator !=(const OrientedBoundingBox& obb) const;
       OrientedBoundingBox& operator =(const OrientedBoundingBox& obb) = default;
    };
}
