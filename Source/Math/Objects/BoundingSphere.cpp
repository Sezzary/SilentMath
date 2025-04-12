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
        return aabb.Intersects(*this);
    }

    bool BoundingSphere::Intersects(const OrientedBoundingBox& obb) const
    {
        return false;
    }
}
