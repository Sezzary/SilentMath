#include "Framework.h"
#include "Math/Objects/AxisAlignedBoundingBox.h"

#include "Math/Constants.h"
#include "Math/Objects/BoundingSphere.h"
#include "Math/Objects/OrientedBoundingBox.h"
#include "Utils/Utils.h"

using namespace Silent::Utils;

namespace Silent::Math
{
    AxisAlignedBoundingBox::AxisAlignedBoundingBox(const std::span<glm::vec3>& points)
    {
        auto pointMin = glm::vec3(INFINITY);
        auto pointMax = glm::vec3(-INFINITY);

        // Compute min and max AABB points.
        for (const auto& point : points)
        {
            pointMin = glm::vec3(
                std::min(pointMin.x, point.x),
                std::min(pointMin.y, point.y),
                std::min(pointMin.z, point.z));

            pointMax = glm::vec3(
                std::max(pointMax.x, point.x),
                std::max(pointMax.y, point.y),
                std::max(pointMax.z, point.z));
        }

        // Construct AABB.
        Center = (pointMin + pointMax) / 2.0f;
        Extents = (pointMax - pointMin) / 2.0f;
    }

    /*AxisAlignedBoundingBox::AxisAlignedBoundingBox(const OrientedBoundingBox& obb)
    {
        *this = AxisAlignedBoundingBox(ToSpan(obb.GetCorners()));
    }*/

    float AxisAlignedBoundingBox::GetSurfaceArea() const
    {
        return ((Extents.x * Extents.y) + (Extents.x * Extents.z) + (Extents.y * Extents.z)) * 2.0f;
    }

    float AxisAlignedBoundingBox::GetVolume() const
    {
        return (Extents.x * Extents.y * Extents.z) * 2.0f;
    }

    std::vector<glm::vec3> AxisAlignedBoundingBox::GetCorners() const
    {
        return std::vector<glm::vec3>
        {
            Center + glm::vec3(-Extents.x, -Extents.y, -Extents.z),
            Center + glm::vec3( Extents.x, -Extents.y, -Extents.z),
            Center + glm::vec3(-Extents.x,  Extents.y, -Extents.z),
            Center + glm::vec3( Extents.x,  Extents.y, -Extents.z),
            Center + glm::vec3(-Extents.x, -Extents.y,  Extents.z),
            Center + glm::vec3( Extents.x, -Extents.y,  Extents.z),
            Center + glm::vec3(-Extents.x,  Extents.y,  Extents.z),
            Center + glm::vec3( Extents.x,  Extents.y,  Extents.z)
        };
    }

    bool AxisAlignedBoundingBox::Intersects(const glm::vec3& point) const
    {
        return point.x >= (Center.x - Extents.x) && point.x <= (Center.x + Extents.x) &&
               point.y >= (Center.y - Extents.y) && point.y <= (Center.y + Extents.y) &&
               point.z >= (Center.z - Extents.z) && point.z <= (Center.z + Extents.z);
    }

    bool AxisAlignedBoundingBox::Intersects(const BoundingSphere& sphere) const
    {
        auto closestPoint = glm::clamp(sphere.Center, Center - Extents, Center + Extents);
        return glm::distance2(closestPoint, sphere.Center) <= SQUARE(sphere.Radius);
    }

    bool AxisAlignedBoundingBox::Intersects(const AxisAlignedBoundingBox& aabb) const
    {
        return (Center.x - Extents.x) <= (aabb.Center.x + aabb.Extents.x) &&
               (Center.x + Extents.x) >= (aabb.Center.x - aabb.Extents.x) &&
               (Center.y - Extents.y) <= (aabb.Center.y + aabb.Extents.y) &&
               (Center.y + Extents.y) >= (aabb.Center.y - aabb.Extents.y) &&
               (Center.z - Extents.z) <= (aabb.Center.z + aabb.Extents.z) &&
               (Center.z + Extents.z) >= (aabb.Center.z - aabb.Extents.z);
    }

    bool AxisAlignedBoundingBox::Intersects(const OrientedBoundingBox& obb) const
    {
        constexpr unsigned int AXIS_COUNT       = 3;
        constexpr float        DEFAULT_PROJ_MIN = std::numeric_limits<float>::max();
        constexpr float        DEFAULT_PROJ_MAX = std::numeric_limits<float>::lowest();

        // Use Separating Axis Theorem (SAT) for AABB vs OBB intersection.
        auto aabbMin = Center - Extents;
        auto aabbMax = Center + Extents;

        // Check for overlap on each axis.
        for (int i = 0; i < AXIS_COUNT; i++)
        {
            auto axis = glm::vec3();

            // X axis.
            if (i == 0)
            {
                axis = glm::vec3(1.0f, 0.0f, 0.0f);
            }

            // Y axis.
            if (i == 1)
            {
                axis = glm::vec3(0.0f, 1.0f, 0.0f);
            }

            // Z axis.
            if (i == 2)
            {
                axis = glm::vec3(0.0f, 0.0f, 1.0f);
            }

            float aabbProjMin = glm::dot(aabbMin, axis);
            float aabbProjMax = glm::dot(aabbMax, axis);

            // Ensure aabbProjMin is always less than aabbProjMax.
            if (aabbProjMin > aabbProjMax)
            {
                std::swap(aabbProjMin, aabbProjMax);
            }

            float obbProjMin = DEFAULT_PROJ_MIN;
            float obbProjMax = DEFAULT_PROJ_MAX;

            // Project OBB corners onto axis i.
            for (const auto& corner : obb.GetCorners())
            {
                float proj = glm::dot(corner, axis);
                obbProjMin = std::min(obbProjMin, proj);
                obbProjMax = std::max(obbProjMax, proj);
            }

            // If no overlap on this axis, no intersection.
            if (aabbProjMax < obbProjMin || obbProjMax < aabbProjMin)
            {
                return false;
            }
        }

        // If no separating axis found, AABB and OBB intersect.
        return true;
    }

    ContainmentType AxisAlignedBoundingBox::Contains(const glm::vec3& point) const
    {
        return Intersects(point) ? ContainmentType::Contains : ContainmentType::Disjoint;
    }

    ContainmentType AxisAlignedBoundingBox::Contains(const BoundingSphere& sphere) const
    {
        auto closestPoint = glm::clamp(sphere.Center, Center - Extents, Center + Extents);
        float distSqr = glm::distance2(closestPoint, sphere.Center);
        if (distSqr > SQUARE(sphere.Radius))
        {
            return ContainmentType::Disjoint;
        }

        auto aabbMin = Center - Extents;
        auto aabbMax = Center + Extents;
        auto sphereMin = sphere.Center - glm::vec3(sphere.Radius);
        auto sphereMax = sphere.Center + glm::vec3(sphere.Radius);

        if (sphereMin.x >= aabbMin.x && sphereMax.x <= aabbMax.x &&
            sphereMin.y >= aabbMin.y && sphereMax.y <= aabbMax.y &&
            sphereMin.z >= aabbMin.z && sphereMax.z <= aabbMax.z)
        {
            return ContainmentType::Contains;
        }

        return ContainmentType::Intersects;
    }

    ContainmentType AxisAlignedBoundingBox::Contains(const AxisAlignedBoundingBox& aabb) const
    {
        if (!Intersects(aabb))
        {
            return ContainmentType::Disjoint;
        }

        auto thisMin = Center - Extents;
        auto thisMax = Center + Extents;
        auto aabbMin = aabb.Center - aabb.Extents;
        auto aabbMax = aabb.Center + aabb.Extents;

        if (aabbMin.x >= thisMin.x && aabbMax.x <= thisMax.x &&
            aabbMin.y >= thisMin.y && aabbMax.y <= thisMax.y &&
            aabbMin.z >= thisMin.z && aabbMax.z <= thisMax.z)
        {
            return ContainmentType::Contains;
        }

        return ContainmentType::Intersects;
    }

    AxisAlignedBoundingBox AxisAlignedBoundingBox::Merge(const AxisAlignedBoundingBox& aabb0, const AxisAlignedBoundingBox& aabb1)
    {
        auto mergedAabb = aabb0;
        mergedAabb.Merge(aabb1);
        return mergedAabb;
    }

    void AxisAlignedBoundingBox::Merge(const AxisAlignedBoundingBox& aabb)
    {
        Center = (Center + aabb.Center) / 2.0f;
        Extents = glm::max(Extents, aabb.Extents);
    }
}
