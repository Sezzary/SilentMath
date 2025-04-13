#include "Framework.h"
#include "Math/Objects/BoundingSphere.h"

#include "Math/Constants.h"
#include "Math/Objects/AxisAlignedBoundingBox.h"

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

    bool BoundingSphere::Intersects(const glm::vec3& point) const
    {
        return glm::distance2(Center, point) <= SQUARE(Radius);
    }

    bool BoundingSphere::Intersects(const BoundingSphere& sphere) const
    {
        return glm::distance2(Center, sphere.Center) <= SQUARE(Radius + sphere.Radius);
    }

    bool BoundingSphere::Intersects(const AxisAlignedBoundingBox& aabb) const
    {
        auto closestPoint = glm::clamp(Center, aabb.Center - aabb.Extents, aabb.Center + aabb.Extents);
        return glm::distance2(closestPoint, Center) <= SQUARE(Radius);
    }

    bool BoundingSphere::Intersects(const OrientedBoundingBox& obb) const
    {
        /*constexpr unsigned int AXIS_COUNT = 3;
    
        auto rotMat = glm::mat3_cast(obb.Rotation);
        auto centerDelta = obb.Center - Center;
        auto closestPoint = obb.Center;

        // Project sphere center onto each axis of OBB.
        for (int i = 0; i < AXIS_COUNT; i++)
        {
            float proj = glm::dot(centerDelta, rotMat[i]);
            proj = glm::clamp(proj, -obb.Extents[i], obb.Extents[i]);
            closestPoint += proj * rotMat[i];
        }

        float distSqr = glm::distance2(Center, closestPoint);
        return distSqr <= SQUARE(Radius);*/
    }
    
    ContainmentType BoundingSphere::Contains(const glm::vec3& point) const
    {
        return Intersects(point) ? ContainmentType::Contains : ContainmentType::Disjoint;
    }

    ContainmentType BoundingSphere::Contains(const BoundingSphere& sphere) const
    {
        float distSqr = glm::distance2(Center, sphere.Center);
        if (distSqr <= ((Radius - sphere.Radius) * (Radius - sphere.Radius)))
        {
            return ContainmentType::Contains;
        }
        
        float radiusSum = Radius + sphere.Radius;
        if (distSqr <= SQUARE(radiusSum))
        {
            return ContainmentType::Intersects;
        }

        return ContainmentType::Disjoint;
    }

    ContainmentType BoundingSphere::Contains(const AxisAlignedBoundingBox& aabb) const
    {
        auto aabbMin = aabb.GetMin();
        auto aabbMax = aabb.GetMax();
    
        auto closestPoint = glm::clamp(Center, aabbMin, aabbMax);
        float distSqr = glm::distance2(closestPoint, Center);
        if (distSqr > SQUARE(Radius))
        {
            return ContainmentType::Disjoint;
        }

        auto sphereMin = Center - glm::vec3(Radius);
        auto sphereMax = Center + glm::vec3(Radius);

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

    }
    
    bool BoundingSphere::operator ==(const BoundingSphere& sphere) const
    {
        return Center == sphere.Center && Radius == sphere.Radius;
    }

    bool BoundingSphere::operator !=(const BoundingSphere& sphere) const
    {
        return !(*this == sphere);
    }
}
