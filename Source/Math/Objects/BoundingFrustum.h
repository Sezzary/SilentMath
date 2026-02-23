#pragma once

#include "Math/Objects/Plane.h"
#include "Math/Objects/Vector3.h"
#include "Math/Objects/Quaternion.h"

namespace Silent::Math
{
    class      AxisAlignedBoundingBox;
    class      BoundingSphere;
    class      OrientedBoundingBox;
    enum class ContainmentType;

    /** @brief 3D bounding frustum. */
    class BoundingFrustum
    {
    public:
        // ==========
        // Constants
        // ==========

        static constexpr int PLANE_COUNT = 6;

        // =======
        // Fields
        // =======

        Vector3    Origin      = Vector3::Zero;
        Quaternion Rotation    = Quaternion::Identity;
        float      RightSlope  = 0.0f;
        float      LeftSlope   = 0.0f;
        float      TopSlope    = 0.0f;
        float      BottomSlope = 0.0f;
        float      Near        = 0.0f;
        float      Far         = 0.0f;

        // =============
        // Constructors
        // =============

        /** @brief Creates a default instance. */
        constexpr BoundingFrustum() = default;

        /** @brief Creates an instance from frustum components.
         *
         * @param origin Frustum origin.
         * @param rot Frustum rotation.
         * @param rightSlope Right slope of the frustum.
         * @param leftSlope Left slope of the frustum.
         * @param topSlope Top slope of the frustum.
         * @param bottomSlope Bottom slope of the frustum.
         * @param near Near plane distance.
         * @param far Far plane distance.
         */
        constexpr BoundingFrustum(const Vector3& origin, const Quaternion& rot,
                                  float rightSlope, float leftSlope, float topSlope, float bottomSlope,
                                  float near, float far) :
            Origin(origin),
            Rotation(rot),
            RightSlope(rightSlope),
            LeftSlope(leftSlope),
            TopSlope(topSlope),
            BottomSlope(bottomSlope),
            Near(near),
            Far(far) {}

        // ==========
        // Inquirers
        // ==========

        /** @brief Checks if a point intersects the frustum.
         *
         * @param point Point to test against.
         * @return `true` if the intersection is valid, `false` otherwise.
         */
        bool Intersects(const Vector3& point) const;

        /** @brief Checks if another sphere intersects the frustum.
         *
         * @param sphere Sphere to test against.
         * @return `true` if the intersection is valid, `false` otherwise.
         */
        bool Intersects(const BoundingSphere& sphere) const;

        /** @brief Checks if an AABB intersects the frustum.
         *
         * @param aabb AABB to test against.
         * @return `true` if the intersection is valid, `false` otherwise.
         */
        bool Intersects(const AxisAlignedBoundingBox& aabb) const;

        /** @brief Checks if an OBB intersects the frustum.
         *
         * @param obb OBB to test against.
         * @return `true` if the intersection is valid, `false` otherwise.
         */
        bool Intersects(const OrientedBoundingBox& obb) const;

        /** @brief Checks if another frustum intersects the frustum.
         *
         * @param frustum Frustum to test against.
         * @return `true` if the intersection is valid, `false` otherwise.
         */
        bool Intersects(const BoundingFrustum& frustum) const;

        /** @brief Checks if a point is contained by the frustum.
         *
         * @param point Point to test for containment.
         * @return Containment type.
         */
        ContainmentType Contains(const Vector3& point) const;

        /** @brief Checks if a sphere is contained by the frustum.
         *
         * @param sphere Sphere to test for containment.
         * @return Containment type.
         */
        ContainmentType Contains(const BoundingSphere& sphere) const;

        /** @brief Checks if an AABB is contained by the frustum.
         *
         * @param aabb AABB to test for containment.
         * @return Containment type.
         */
        ContainmentType Contains(const AxisAlignedBoundingBox& aabb) const;

        /** @brief Checks if an OBB is contained by the frustum.
         *
         * @param obb OBB to test for containment.
         * @return Containment type.
         */
        ContainmentType Contains(const OrientedBoundingBox& obb) const;

        /** @brief Checks if another frustum is contained by the frustum.
         *
         * @param frustum Frustum to test for containment.
         * @return Containment type.
         */
        ContainmentType Contains(const BoundingFrustum& frustum) const;

        // ==========
        // Operators
        // ==========

        bool             operator==(const BoundingFrustum& frustum) const;
        bool             operator!=(const BoundingFrustum& frustum) const;
        BoundingFrustum& operator=(const BoundingFrustum& frustum) = default;

    private:
        // ========
        // Helpers
        // ========

        /** @brief Computes the planes of the frustum.
         *
         * @return Frustum planes.
         */
        std::array<Plane, PLANE_COUNT> GetPlanes() const;
    };
}
