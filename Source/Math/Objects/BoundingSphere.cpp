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
        return false;
    }
    
    /*ContainmentType BoundingSphere::Contains(const glm::vec3& point) const
    {
        
    }

    ContainmentType BoundingSphere::Contains(const BoundingSphere& sphere) const
    {
        
    }

    ContainmentType BoundingSphere::Contains(const AxisAlignedBoundingBox& aabb) const
    {
        
    }

    ContainmentType BoundingSphere::Contains(const OrientedBoundingBox& obb) const
    {

    }*/
}
