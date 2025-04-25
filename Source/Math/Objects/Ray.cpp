#include "Framework.h"
#include "Math/Objects/Ray.h"

#include "Math/Constants.h"
#include "Math/Objects/AxisAlignedBoundingBox.h"
#include "Math/Objects/BoundingSphere.h"
#include "Math/Objects/OrientedBoundingBox.h"

namespace Silent::Math
{
    std::optional<float> Ray::Intersects(const BoundingSphere& sphere) const
    {
        auto posDelta = sphere.Center - Origin;
        auto projLength = glm::dot(posDelta, Direction);

        auto distSqr = glm::length2(posDelta) - SQUARE(projLength);
        auto radiusSqr = SQUARE(sphere.Radius);
        if (distSqr > radiusSqr)
        {
            return std::nullopt;
        }
    
        auto intersectOffset = sqrt(radiusSqr - distSqr);
        return projLength - intersectOffset;
    }

    std::optional<float> Ray::Intersects(const AxisAlignedBoundingBox& aabb) const
    {
        auto invDir = 1.0f / Direction;
        auto intersectMin = ((aabb.Center - aabb.Extents) - Origin) * invDir;
        auto intersectMax = ((aabb.Center + aabb.Extents) - Origin) * invDir;

        float nearIntersect = std::max({ intersectMin.x, intersectMin.y, intersectMin.z });
        float farIntersect = std::min({ intersectMax.x, intersectMax.y, intersectMax.z });
        if (nearIntersect > farIntersect || farIntersect < 0.0f)
        {
            return std::nullopt;
        }

        return nearIntersect;
    }

    std::optional<float> Ray::Intersects(const OrientedBoundingBox& obb) const
    {
        // Compute inverse OBB transform matrix.
        auto invTransformMat = glm::inverse(obb.GetTransformMatrix());

        // Compute local ray.
        auto localOrigin = glm::vec3(glm::vec4(Origin, 1.0f) * invTransformMat);
        auto localDir = glm::vec3(glm::vec4(Direction, 0.0f) * invTransformMat);
        auto localRay = Ray(localOrigin, localDir);

        // Test AABB intersection in local space.
        auto aabb = AxisAlignedBoundingBox(glm::vec3(0.0f), obb.Extents);
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
