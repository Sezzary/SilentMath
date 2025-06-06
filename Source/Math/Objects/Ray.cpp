#include "Framework.h"
#include "Math/Objects/Ray.h"

#include "Math/Constants.h"
#include "Math/Objects/AxisAlignedBoundingBox.h"
#include "Math/Objects/BoundingSphere.h"
#include "Math/Objects/Matrix.h"
#include "Math/Objects/OrientedBoundingBox.h"
#include "Math/Objects/Vector3.h"

namespace Silent::Math
{
    std::optional<float> Ray::Intersects(const BoundingSphere& sphere) const
    {
        auto  posDelta   = sphere.Center - Origin;
        float projLength = Vector3::Dot(posDelta, Direction);

        float distSqr   = posDelta.LengthSquared() - SQUARE(projLength);
        float radiusSqr = SQUARE(sphere.Radius);
        if (distSqr > radiusSqr)
        {
            return std::nullopt;
        }
    
        float intersectOffset = sqrt(radiusSqr - distSqr);
        return projLength - intersectOffset;
    }

    std::optional<float> Ray::Intersects(const AxisAlignedBoundingBox& aabb) const
    {
        auto invDir       = Vector3::One / Direction;
        auto intersectMin = ((aabb.Center - aabb.Extents) - Origin) * invDir;
        auto intersectMax = ((aabb.Center + aabb.Extents) - Origin) * invDir;

        float nearIntersect = std::max({ intersectMin.x, intersectMin.y, intersectMin.z });
        float farIntersect  = std::min({ intersectMax.x, intersectMax.y, intersectMax.z });
        if (nearIntersect > farIntersect || farIntersect < 0.0f)
        {
            return std::nullopt;
        }

        return nearIntersect;
    }

    std::optional<float> Ray::Intersects(const OrientedBoundingBox& obb) const
    {
        // Compute inverse OBB transform matrix.
        auto invTransformMat = Matrix::Inverse(obb.GetTransformMatrix());

        // Compute local ray.
        auto localOrigin = Vector3::Transform(Origin, invTransformMat);
        auto localDir    = Vector3::Rotate(Direction, invTransformMat);
        auto localRay    = Ray(localOrigin, localDir);

        // Test AABB intersection in local space.
        auto aabb = AxisAlignedBoundingBox(Vector3::Zero, obb.Extents);
        return localRay.Intersects(aabb);
    }

    bool Ray::operator==(const Ray& ray) const
    {
        return Origin == ray.Origin && Direction == ray.Direction;
    }

    bool Ray::operator!=(const Ray& ray) const
    {
        return !(*this == ray);
    }
}
