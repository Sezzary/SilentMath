#pragma once

#include "Math/Objects/Quaternion.h"
#include "Math/Objects/Vector3.h"

namespace Silent::Math
{
    class      AxisAlignedBoundingBox;
    class      BoundingSphere;
    class      Matrix;
    enum class ContainmentType;

    /** @brief 3D oriented bounding box (OBB). */
    class OrientedBoundingBox
    {
    public:
        // ==========
        // Constants
        // ==========

        static constexpr uint CORNER_COUNT = 8;
        static constexpr uint EDGE_COUNT   = 12;
        static constexpr uint FACE_COUNT   = 6;

        // =======
        // Fields
        // =======

        Vector3    Center   = Vector3::Zero;
        Vector3    Extents  = Vector3::One;
        Quaternion Rotation = Quaternion::Identity;

        // =============
        // Constructors
        // =============

        /** @brief Constructs a default `OrientedBoundingBox`. */
        constexpr OrientedBoundingBox() = default;

        /** @brief Constructs a `OrientedBoundingBox` from a center, extents, and rotation.
         *
         * @param center Center.
         * @param extents Extents.
         * @param rot Rotation.
         */
        constexpr OrientedBoundingBox(const Vector3& center, const Vector3& extents, const Quaternion rot) : Center(center), Extents(extents), Rotation(rot) {}

        // ========
        // Getters
        // ========

        /** @brief Gets the X-axis width of the OBB.
         *
         * @return Width.
         */
        float GetWidth() const;

        /** @brief Gets the Y-axis height of the OBB.
         *
         * @return Height.
         */
        float GetHeight() const;

        /** @brief Gets the Z-axis depth of the OBB.
         *
         * @return Ddepth.
         */
        float GetDepth() const;

        /** @brief Gets the surface area of the OBB.
         *
         * @return Surface area.
         */
        float GetSurfaceArea() const;

        /** @brief Gets the volume of the OBB.
         *
         * @return Volume.
         */
        float GetVolume() const;

        /** @brief Gets the 8 corner points of the OBB.
         *
         * @return Corner points.
         */
        std::array<Vector3, CORNER_COUNT> GetCorners() const;

        /** @brief Gets the transform matrix of the OBB.
         *
         * @return Transform matrix.
         */
        Matrix GetTransformMatrix() const;

        // ==========
        // Utilities
        // ==========

        /** @brief Checks if a point intersects the OBB.
         *
         * @param point Point to test against.
         * @return `true` if the intersection is valid, `false` otherwise.
         */
        bool Intersects(const Vector3& point) const;

        /** @brief Checks if a sphere intersects the OBB.
         *
         * @param sphere Sphere to test against.
         * @return `true` if the intersection is valid, `false` otherwise.
         */
        bool Intersects(const BoundingSphere& sphere) const;

        /** @brief Checks if an AABB intersects the OBB.
         *
         * @param aabb AABB to test against.
         * @return `true` if the intersection is valid, `false` otherwise.
         */
        bool Intersects(const AxisAlignedBoundingBox& aabb) const;

        /** @brief Checks if another OBB intersects the OBB.
         *
         * @param obb OBB to test against.
         * @return `true` if the intersection is valid, `false` otherwise.
         */
        bool Intersects(const OrientedBoundingBox& obb) const;

        /** @brief Checks if a point is contained by the OBB.
         *
         * @param point Point to test for containment.
         * @return Containment type.
         */
        ContainmentType Contains(const Vector3& point) const;

        /** @brief Checks if a sphere is contained by the OBB.
         *
         * @param sphere Sphere to test for containment.
         * @return Containment type.
         */
        ContainmentType Contains(const BoundingSphere& sphere) const;

        /** @brief Checks if an AABB is contained by the OBB.
         *
         * @param aabb AABB to test for containment.
         * @return Containment type.
         */
        ContainmentType Contains(const AxisAlignedBoundingBox& aabb) const;

        /** @brief Checks if another OBB is contained by the OBB.
         *
         * @param obb OBB to test for containment.
         * @return Containment type.
         */
        ContainmentType Contains(const OrientedBoundingBox& obb) const;

        // ===========
        // Converters
        // ===========

        /** Converts the OBB to a sphere.
         *
         * @return Sphere encompassing the OBB.
         */
        BoundingSphere ToSphere() const;

        /** Converts the OBB to an AABB.
         *
         * @return AABB encompassing the OBB.
         */
        AxisAlignedBoundingBox ToAabb() const;

        // ==========
        // Operators
        // ==========

        bool                 operator==(const OrientedBoundingBox& obb) const;
        bool                 operator!=(const OrientedBoundingBox& obb) const;
        OrientedBoundingBox& operator=(const OrientedBoundingBox& obb) = default;
    };
}
