#include "Framework.h"
#include "Math/Objects/OrientedBoundingBox.h"

#include "Math/Constants.h"
#include "Math/Objects/AxisAlignedBoundingBox.h"
#include "Math/Objects/BoundingSphere.h"

namespace Silent::Math
{
    float OrientedBoundingBox::GetSurfaceArea() const
    {
        return ((Extents.x * Extents.y) + (Extents.x * Extents.z) + (Extents.y * Extents.z)) * 2.0f;
    }

    float OrientedBoundingBox::GetVolume() const
    {
        return Extents.x * Extents.y * Extents.z;
    }

    std::vector<glm::vec3> OrientedBoundingBox::GetCorners() const
    {
        auto rotMat = glm::mat4_cast(Rotation);

        return std::vector<glm::vec3>
        {
            Center + glm::vec3(glm::vec4(-Extents.x, -Extents.y, -Extents.z, 1.0f) * rotMat),
            Center + glm::vec3(glm::vec4( Extents.x, -Extents.y, -Extents.z, 1.0f) * rotMat),
            Center + glm::vec3(glm::vec4(-Extents.x,  Extents.y, -Extents.z, 1.0f) * rotMat),
            Center + glm::vec3(glm::vec4( Extents.x,  Extents.y, -Extents.z, 1.0f) * rotMat),
            Center + glm::vec3(glm::vec4(-Extents.x, -Extents.y,  Extents.z, 1.0f) * rotMat),
            Center + glm::vec3(glm::vec4( Extents.x, -Extents.y,  Extents.z, 1.0f) * rotMat),
            Center + glm::vec3(glm::vec4(-Extents.x,  Extents.y,  Extents.z, 1.0f) * rotMat),
            Center + glm::vec3(glm::vec4( Extents.x,  Extents.y,  Extents.z, 1.0f) * rotMat)
        };
    }

    glm::mat4 OrientedBoundingBox::GetTransformMatrix() const
    {
        auto translationMat = glm::translate(glm::mat4(1.0f), Center);
        auto rotMat = glm::mat4_cast(Rotation);
    
        return rotMat * translationMat;
    }
    
    bool OrientedBoundingBox::Intersects(const glm::vec3& point) const
    {
        auto rotMat = glm::mat3_cast(Rotation);
        auto localPoint = (point - Center) * rotMat;

        return std::abs(localPoint.x) <= Extents.x && 
               std::abs(localPoint.y) <= Extents.y && 
               std::abs(localPoint.z) <= Extents.z;
    }

    bool OrientedBoundingBox::Intersects(const BoundingSphere& sphere) const
    {
        return sphere.Intersects(*this);
    }

    bool OrientedBoundingBox::Intersects(const AxisAlignedBoundingBox& aabb) const
    {
        return aabb.Intersects(*this);
    }

    bool OrientedBoundingBox::Intersects(const OrientedBoundingBox& obb) const
    {
        constexpr int AXIS_COUNT = 3;

        // Compute rotation matrices.
        auto rotMat0 = glm::mat3_cast(Rotation);
        auto rotMat1 = glm::mat3_cast(obb.Rotation);

        // Compute center delta.
        auto centerDelta = obb.Center - Center;

        // Test all the axes. 8 total: 3 from OBB 0, and 3 from OBB 1, and 2 from cross products of axes.
        for (int i = 0; i < AXIS_COUNT; i++)
        {
            for (int j = 0; j < AXIS_COUNT; j++)
            {
                auto axis = glm::normalize(glm::cross(rotMat0[i], rotMat1[j]));

                // Project all vertices of both OBBs onto axis.
                float proj0 = -INFINITY;
                float proj1 = INFINITY;
                
                // Project OBB 0 vertices.
                for (const auto& corner : GetCorners())
                {
                    float proj = glm::dot(corner, axis);
                    proj0 = std::max(proj0, proj);
                    proj1 = std::min(proj1, proj);
                }

                // Project OBB 1 vertices.
                float proj3 = -INFINITY;
                float proj4 = INFINITY;
                for (const auto& corner : obb.GetCorners())
                {
                    float proj = glm::dot(corner, axis);
                    proj3 = std::max(proj3, proj);
                    proj4 = std::min(proj4, proj);
                }

                // Check if projections overlap.
                if (proj0 < proj4 || proj3 < proj1)
                {
                    return false;
                }
            }
        }

        return true;
    }

    ContainmentType OrientedBoundingBox::Contains(const glm::vec3& point) const
    {
        return Intersects(point) ? ContainmentType::Contains : ContainmentType::Disjoint;
    }

    ContainmentType OrientedBoundingBox::Contains(const BoundingSphere& sphere) const
    {
        return sphere.Contains(*this);
    }

    ContainmentType OrientedBoundingBox::Contains(const AxisAlignedBoundingBox& aabb) const
    {
        return aabb.Contains(*this);
    }

    ContainmentType OrientedBoundingBox::Contains(const OrientedBoundingBox& obb) const
    {
        // Check if each corner of other OBB is inside this OBB.
        for (const auto& corner : obb.GetCorners())
        {
            // Transform corner to local space of this OBB using inverse rotation.
            auto rotCorner = glm::inverse(Rotation) * (corner - Center);

            // Check if corner lies within extents of this OBB in all axes.
            bool isInside = false;
            if (glm::abs(rotCorner.x) <= Extents.x &&
                glm::abs(rotCorner.y) <= Extents.y &&
                glm::abs(rotCorner.z) <= Extents.z)
            {
                isInside = true;
            }

            // If any corner is outside, this OBB doesn't fully contain other OBB.
            if (!isInside)
            {
                return ContainmentType::Disjoint;
            }
        }

        // All corners of other OBB are inside this OBB.
        return ContainmentType::Contains;
    }

    bool OrientedBoundingBox::operator ==(const OrientedBoundingBox& obb) const
    {
        return Center == obb.Center && Extents == obb.Extents && Rotation == obb.Rotation;
    }

    bool OrientedBoundingBox::operator !=(const OrientedBoundingBox& obb) const
    {
        return !(*this == obb);
    }
}
