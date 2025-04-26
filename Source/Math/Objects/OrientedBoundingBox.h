#pragma once

namespace Silent::Math
{
    class      AxisAlignedBoundingBox;
    class      BoundingSphere;
    class      Vector3;
    enum class ContainmentType;

    class OrientedBoundingBox
    {
    public:
        // Constants

        static constexpr uint CORNER_COUNT = 8;

        // Fields

        Vector3 Center     = Vector3();
        Vector3 Extents    = Vector3();
        glm::quat Rotation = glm::quat();

        // Constructors

       constexpr OrientedBoundingBox() = default;
       constexpr OrientedBoundingBox(const Vector3& center, const Vector3& extents, const glm::quat rot) : Center(center), Extents(extents), Rotation(rot) {}

       // Getters

       float                GetWidth() const;
       float                GetHeight() const;
       float                GetDepth() const;
       float                GetSurfaceArea() const;
       float                GetVolume() const;
       std::vector<Vector3> GetCorners() const;
       glm::mat4            GetTransformMatrix() const;

       // Utilities

       bool Intersects(const Vector3& point) const;
       bool Intersects(const BoundingSphere& sphere) const;
       bool Intersects(const AxisAlignedBoundingBox& aabb) const;
       bool Intersects(const OrientedBoundingBox& obb) const;

       ContainmentType Contains(const Vector3& point) const;
       ContainmentType Contains(const BoundingSphere& sphere) const;
       ContainmentType Contains(const AxisAlignedBoundingBox& aabb) const;
       ContainmentType Contains(const OrientedBoundingBox& obb) const;

       // Operators

       bool                 operator==(const OrientedBoundingBox& obb) const;
       bool                 operator!=(const OrientedBoundingBox& obb) const;
       OrientedBoundingBox& operator=(const OrientedBoundingBox& obb) = default;
    };
}
