#include "Framework.h"
#include "Math/Objects/BoundingSphere.h"

#include "Math/Constants.h"
#include "Math/Objects/AxisAlignedBoundingBox.h"
#include "Math/Objects/OrientedBoundingBox.h"
#include "Math/Objects/Vector3.h"

namespace Silent::Math
{
    float BoundingSphere::GetSurfaceArea() const
    {
        return SQUARE(Radius) * PI_MUL_4;
    }

    float BoundingSphere::GetVolume() const
    {
        return CUBE(Radius) * (PI * (4.0f / 3.0f));
    }

    bool BoundingSphere::Intersects(const Vector3& point) const
    {
        return Vector3::DistanceSquared(Center, point) <= SQUARE(Radius);
    }

    bool BoundingSphere::Intersects(const BoundingSphere& sphere) const
    {
        return Vector3::DistanceSquared(Center, sphere.Center) <= SQUARE(Radius + sphere.Radius);
    }

    bool BoundingSphere::Intersects(const AxisAlignedBoundingBox& aabb) const
    {
        auto closestPoint = Vector3::Clamp(Center, aabb.Center - aabb.Extents, aabb.Center + aabb.Extents);
        return Vector3::DistanceSquared(closestPoint, Center) <= SQUARE(Radius);
    }

    bool BoundingSphere::Intersects(const OrientedBoundingBox& obb) const
    {
        auto rotMat = glm::mat3_cast(obb.Rotation);
        auto centerDelta = obb.Center - Center;
        auto closestPoint = obb.Center;

        // Project sphere center onto each axis of OBB.
        /*for (int i = 0; i < Vector3::AXIS_COUNT; i++)
        {
            float proj = Vector3::Dot(centerDelta, rotMat[i]);
            proj = glm::clamp(proj, -obb.Extents[i], obb.Extents[i]);
            closestPoint += proj * rotMat[i];
        }*/

        float distSqr = Vector3::DistanceSquared(Center, closestPoint);
        return distSqr <= SQUARE(Radius);
    }
    
    ContainmentType BoundingSphere::Contains(const Vector3& point) const
    {
        return Intersects(point) ? ContainmentType::Contains : ContainmentType::None;
    }

    ContainmentType BoundingSphere::Contains(const BoundingSphere& sphere) const
    {
        float distSqr = Vector3::DistanceSquared(Center, sphere.Center);
        if (distSqr <= ((Radius - sphere.Radius) * (Radius - sphere.Radius)))
        {
            return ContainmentType::Contains;
        }
        
        float radiusSum = Radius + sphere.Radius;
        if (distSqr <= SQUARE(radiusSum))
        {
            return ContainmentType::Intersects;
        }

        return ContainmentType::None;
    }

    ContainmentType BoundingSphere::Contains(const AxisAlignedBoundingBox& aabb) const
    {
        auto aabbMin = aabb.GetMin();
        auto aabbMax = aabb.GetMax();
    
        auto closestPoint = Vector3::Clamp(Center, aabbMin, aabbMax);
        float distSqr = Vector3::DistanceSquared(closestPoint, Center);
        if (distSqr > SQUARE(Radius))
        {
            return ContainmentType::None;
        }

        auto sphereMin = Center - Vector3(Radius);
        auto sphereMax = Center + Vector3(Radius);

        if (sphereMin.x >= aabbMin.x && sphereMax.x <= aabbMax.x &&
            sphereMin.y >= aabbMin.y && sphereMax.y <= aabbMax.y &&
            sphereMin.z >= aabbMin.z && sphereMax.z <= aabbMax.z)
        {
            return ContainmentType::Contains;
        }

        return ContainmentType::Intersects;
    }

    ContainmentType BoundingSphere::Contains(const OrientedBoundingBox& obb) const
    {
        bool allInside = true;
        for (const auto& corner : obb.GetCorners())
        {
            auto centerDelta = corner - Center;
            float distSqr = Vector3::Dot(centerDelta, centerDelta);
            if (distSqr > SQUARE(Radius))
            {
                allInside = false;
                break;
            }
        }

        if (allInside)
        {
            return ContainmentType::Contains;
        }

        if (obb.Intersects(Center))
        {
            return ContainmentType::Intersects;
        }

        return ContainmentType::None;
    }
    
    bool BoundingSphere::operator==(const BoundingSphere& sphere) const
    {
        return Center == sphere.Center && Radius == sphere.Radius;
    }

    bool BoundingSphere::operator!=(const BoundingSphere& sphere) const
    {
        return !(*this == sphere);
    }
}
