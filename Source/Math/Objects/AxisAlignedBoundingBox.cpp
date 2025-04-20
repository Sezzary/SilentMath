#include "Framework.h"
#include "Math/Objects/AxisAlignedBoundingBox.h"

#include "Math/Constants.h"
#include "Math/Objects/BoundingSphere.h"
#include "Math/Objects/OrientedBoundingBox.h"
#include "Utils/Utils.h"

using namespace Silent::Utils;

namespace Silent::Math
{
    AxisAlignedBoundingBox::AxisAlignedBoundingBox(const std::span<const glm::vec3>& points)
    {
        auto pointMin = glm::vec3(INFINITY);
        auto pointMax = glm::vec3(-INFINITY);

        // Compute min and max AABB points.
        for (const auto& point : points)
        {
            pointMin = glm::min(pointMin, point);
            pointMax = glm::max(pointMax, point);
        }

        // Construct AABB.
        Center = (pointMin + pointMax) / 2.0f;
        Extents = (pointMax - pointMin) / 2.0f;
    }

    AxisAlignedBoundingBox::AxisAlignedBoundingBox(const BoundingSphere& sphere)
    {
        Center = sphere.Center;
        Extents = glm::vec3(sphere.Radius);
    }

    AxisAlignedBoundingBox::AxisAlignedBoundingBox(const OrientedBoundingBox& obb)
    {
        *this = AxisAlignedBoundingBox(ToSpan(obb.GetCorners()));
    }

    float AxisAlignedBoundingBox::GetWidth() const
    {
        return Extents.x * 2;
    }

    float AxisAlignedBoundingBox::GetHeight() const
    {
        return Extents.y * 2;
    }

    float AxisAlignedBoundingBox::GetDepth() const
    {
        return Extents.z * 2;
    }

    float AxisAlignedBoundingBox::GetSurfaceArea() const
    {
        return ((Extents.x * Extents.y) + (Extents.x * Extents.z) + (Extents.y * Extents.z)) * 2.0f;
    }

    float AxisAlignedBoundingBox::GetVolume() const
    {
        return (Extents.x * Extents.y * Extents.z) * 2.0f;
    }

    glm::vec3 AxisAlignedBoundingBox::GetMin() const
    {
        return Center - Extents;
    }

    glm::vec3 AxisAlignedBoundingBox::GetMax() const
    {
        return Center + Extents;
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
        return sphere.Intersects(*this);
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
        constexpr uint  AXIS_COUNT       = 3;
        constexpr float DEFAULT_PROJ_MIN = std::numeric_limits<float>::max();
        constexpr float DEFAULT_PROJ_MAX = std::numeric_limits<float>::lowest();

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
            else if (i == 1)
            {
                axis = glm::vec3(0.0f, 1.0f, 0.0f);
            }
            // Z axis.
            else if (i == 2)
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
        return sphere.Contains(*this);
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

    ContainmentType AxisAlignedBoundingBox::Contains(const OrientedBoundingBox& obb) const
    {
        // Get the corners of the OBB.
        
        bool allInside = true;
        bool allOutside = true;

        // Check if each OBB corner is inside AABB.
        auto obbCorners = obb.GetCorners();
        for (const auto& corner : obbCorners)
        {
            // Is inside.
            if (corner.x >= (Center.x - Extents.x) && corner.x <= (Center.x + Extents.x) &&
                corner.y >= (Center.y - Extents.y) && corner.y <= (Center.y + Extents.y) &&
                corner.z >= (Center.z - Extents.z) && corner.z <= (Center.z + Extents.z))
            {
                allOutside = false;
            }
            // Is outside.
            else
            {
                allInside = false;
            }
        }

        // Determine containment type based on corner checks.
        if (allInside)
        {
            return ContainmentType::Contains;
        }
        else if (allOutside)
        {
            return ContainmentType::Disjoint;
        }
        else
        {
            return ContainmentType::Intersects;
        }
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

    bool AxisAlignedBoundingBox::operator ==(const AxisAlignedBoundingBox& aabb) const
    {
        return Center == aabb.Center && Extents == aabb.Extents;
    }

    bool AxisAlignedBoundingBox::operator !=(const AxisAlignedBoundingBox& aabb) const
    {
        return !(*this == aabb);
    }
}
