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
        auto posDelta = sphere.Center - Position;
        auto projLength = glm::dot(posDelta, Direction);

        auto distSqr = glm::length2(posDelta) - SQUARE(projLength);
        auto radiusSqr = SQUARE(sphere.Radius);
        if (distSqr > radiusSqr)
        {
            return std::nullopt;
        }
    
        auto intersectOffset = sqrt(radiusSqr - distSqr);
        return std::make_optional(projLength - intersectOffset);
    }
    
    std::optional<float> Ray::Intersects(const AxisAlignedBoundingBox& aabb) const
    {
        auto invDir = 1.0f / Direction;
        auto intersectMin = (aabb.Center - aabb.Extents - Position) * invDir;
        auto intersectMax = (aabb.Center + aabb.Extents - Position) * invDir;

        float nearIntersect = std::max(std::max(intersectMin.x, intersectMin.y), intersectMin.z);
        float farIntersect = std::min(std::min(intersectMax.x, intersectMax.y), intersectMax.z);
        if (nearIntersect > farIntersect || farIntersect < 0.0f)
        {
            return std::nullopt;
        }
        
        return std::make_optional(nearIntersect);
    }

    std::optional<float> Ray::Intersects(const OrientedBoundingBox& obb) const
    {
        // Compute OBB's inverse transform matrix
        auto invTransformMatrix = glm::inverse(obb.GetTransformMatrix());

        // Compute local ray.
        auto localPos = glm::vec3(glm::vec4(Position, 1.0f) * invTransformMatrix);
        auto localDir = glm::vec3(glm::vec4(Direction, 0.0f) * invTransformMatrix);
        auto localRay = Ray(localPos, localDir);

        // Test AABB intersection in local space.
        auto aabb = AxisAlignedBoundingBox(glm::vec3(0.0f), obb.Extents);
        return localRay.Intersects(aabb);
    }

    bool Ray::operator ==(const Ray& ray) const
    {
        return Position == ray.Position && Direction == ray.Direction;
    }

    bool Ray::operator !=(const Ray& ray) const
    {
        return !(*this == ray);
    }
}
