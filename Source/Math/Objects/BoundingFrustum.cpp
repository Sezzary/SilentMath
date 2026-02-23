#include "Framework.h"
#include "Math/Objects/BoundingFrustum.h"

#include "Math/Constants.h"
#include "Math/Objects/AxisAlignedBoundingBox.h"
#include "Math/Objects/BoundingSphere.h"
#include "Math/Objects/OrientedBoundingBox.h"
#include "Math/Objects/Plane.h"
#include "Math/Objects/Vector3.h"

namespace Silent::Math
{
    bool BoundingFrustum::Intersects(const Vector3& point) const
    {
        auto localPoint = Vector3::Transform(point - Origin, Matrix::Inverse(Rotation.ToRotationMatrix()));

        // Check Z-depth, assuming +Z is forward.
        if (localPoint.z < Near || localPoint.z > Far)
        {
            return false;
        }

        // Check horizontal bounds.
        if (localPoint.x > (RightSlope * localPoint.z) ||
            localPoint.x < (LeftSlope  * localPoint.z))
        {
            return false;
        }

        // Check vertical bounds.
        if (localPoint.y > (TopSlope    * localPoint.z) ||
            localPoint.y < (BottomSlope * localPoint.z))
        {
            return false;
        }

        return true;
    }

    bool BoundingFrustum::Intersects(const BoundingSphere& sphere) const
    {
        auto localCenter = Vector3::Transform(sphere.Center - Origin, Matrix::Inverse(Rotation.ToRotationMatrix()));

        // Check Z-depth with radius.
        if (localCenter.z < (Near - sphere.Radius) ||
            localCenter.z > (Far  + sphere.Radius))
        {
            return false;
        }

        // Check planes using "distance to plane" logic via slopes.
        float limitX = RightSlope * localCenter.z;
        if (std::abs(localCenter.x) > (limitX + sphere.Radius))
        {
            return false;
        }

        if (std::abs(localCenter.y) > ((TopSlope * localCenter.z) + sphere.Radius))
        {
            return false;
        }

        return true;
    }

    bool BoundingFrustum::Intersects(const AxisAlignedBoundingBox& aabb) const
    {
        // @todo
        return false;
    }

    bool BoundingFrustum::Intersects(const OrientedBoundingBox& obb) const
    {
        // @todo
        return false;
    }

    ContainmentType BoundingFrustum::Contains(const Vector3& point) const
    {
        return Intersects(point) ? ContainmentType::Contains : ContainmentType::None;
    }

    ContainmentType BoundingFrustum::Contains(const BoundingSphere& sphere) const
    {
        auto planes    = GetPlanes(); 
        bool allInside = true;

        for (const auto& plane : planes)
        {
            float dist = Vector3::Dot(plane.Normal, sphere.Center) + plane.Distance;
            if (dist < -sphere.Radius)
            {
                return ContainmentType::None;
            }

            if (dist < sphere.Radius)
            {
                allInside = false;
            }
        }

        return allInside ? ContainmentType::Contains : ContainmentType::Intersects;
    }

    ContainmentType BoundingFrustum::Contains(const AxisAlignedBoundingBox& aabb) const
    {
        // @todo
        return ContainmentType::None;
    }

    ContainmentType BoundingFrustum::Contains(const OrientedBoundingBox& obb) const
    {
        // @todo
        return ContainmentType::None;
    }

    ContainmentType BoundingFrustum::Contains(const BoundingFrustum& frustum) const
    {
        // @todo
        return ContainmentType::None;
    }

    std::array<Plane, BoundingFrustum::PLANE_COUNT> BoundingFrustum::GetPlanes() const
    {
        auto rotMat = Rotation.ToRotationMatrix();

        auto ComputePlane = [&](Vector3 localNormal, float localPointOnPlaneZ)
        {
            auto worldNormal = Vector3::Transform(localNormal, rotMat);
            worldNormal.Normalize();

            auto localPoint = (localPointOnPlaneZ == 0.0f) ? Vector3::Zero : Vector3(0.0f, 0.0f, localPointOnPlaneZ);
            auto worldPoint = Vector3::Transform(localPoint, rotMat) + Origin;

            float dist = -Vector3::Dot(worldNormal, worldPoint);
            return Plane(worldNormal, dist);
        };

        return std::array<Plane, PLANE_COUNT>
        {
            ComputePlane( Vector3( 1.0f,  0.0f, -LeftSlope),   0.0f),
            ComputePlane( Vector3(-1.0f,  0.0f,  RightSlope),  0.0f),
            ComputePlane( Vector3( 0.0f, -1.0f,  TopSlope),    0.0f),
            ComputePlane( Vector3( 0.0f,  1.0f, -BottomSlope), 0.0f),
            ComputePlane( Vector3::UnitZ, Near),
            ComputePlane(-Vector3::UnitZ, Far)
        };
    }
}
