#include "Framework.h"
#include "Math/Objects/OrientedBoundingBox.h"

#include "Math/Constants.h"
#include "Math/Objects/AxisAlignedBoundingBox.h"
#include "Math/Objects/BoundingSphere.h"
#include "Math/Objects/Matrix.h"

namespace Silent::Math
{
    float OrientedBoundingBox::GetWidth() const
    {
        return Extents.x * 2;
    }

    float OrientedBoundingBox::GetHeight() const
    {
        return Extents.y * 2;
    }

    float OrientedBoundingBox::GetDepth() const
    {
        return Extents.z * 2;
    }

    float OrientedBoundingBox::GetSurfaceArea() const
    {
        return ((Extents.x * Extents.y) + (Extents.x * Extents.z) + (Extents.y * Extents.z)) * 2.0f;
    }

    float OrientedBoundingBox::GetVolume() const
    {
        return Extents.x * Extents.y * Extents.z;
    }

    std::vector<Vector3> OrientedBoundingBox::GetCorners() const
    {
        auto rotMat = glm::mat4_cast(Rotation);

        return std::vector<Vector3>
        {
            Center + Vector3(glm::vec4(-Extents.x, -Extents.y, -Extents.z, 1.0f) * rotMat),
            Center + Vector3(glm::vec4( Extents.x, -Extents.y, -Extents.z, 1.0f) * rotMat),
            Center + Vector3(glm::vec4(-Extents.x,  Extents.y, -Extents.z, 1.0f) * rotMat),
            Center + Vector3(glm::vec4( Extents.x,  Extents.y, -Extents.z, 1.0f) * rotMat),
            Center + Vector3(glm::vec4(-Extents.x, -Extents.y,  Extents.z, 1.0f) * rotMat),
            Center + Vector3(glm::vec4( Extents.x, -Extents.y,  Extents.z, 1.0f) * rotMat),
            Center + Vector3(glm::vec4(-Extents.x,  Extents.y,  Extents.z, 1.0f) * rotMat),
            Center + Vector3(glm::vec4( Extents.x,  Extents.y,  Extents.z, 1.0f) * rotMat)
        };
    }

    Matrix OrientedBoundingBox::GetTransformMatrix() const
    {
        auto translationMat = glm::translate(glm::mat4(1.0f), Center);
        auto rotMat = glm::mat4_cast(Rotation);
    
        return rotMat * translationMat;
    }
    
    bool OrientedBoundingBox::Intersects(const Vector3& point) const
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
        // Compute rotation matrices.
        auto rotMat0 = glm::mat3_cast(Rotation);
        auto rotMat1 = glm::mat3_cast(obb.Rotation);

        // Compute center delta.
        auto centerDelta = obb.Center - Center;

        // Test all the axes. 8 total: 3 from OBB 0, and 3 from OBB 1, and 2 from cross products of axes.
        for (int i = 0; i < Vector3::AXIS_COUNT; i++)
        {
            for (int j = 0; j < Vector3::AXIS_COUNT; j++)
            {
                auto axis = glm::normalize(glm::cross(rotMat0[i], rotMat1[j]));

                // Project all vertices of both OBBs onto axis.
                float proj0 = -INFINITY;
                float proj1 = INFINITY;
                
                // Project OBB 0 vertices.
                for (const auto& corner : GetCorners())
                {
                    float proj = Vector3::Dot(corner, axis);
                    proj0 = std::max(proj0, proj);
                    proj1 = std::min(proj1, proj);
                }

                // Project OBB 1 vertices.
                float proj3 = -INFINITY;
                float proj4 = INFINITY;
                for (const auto& corner : obb.GetCorners())
                {
                    float proj = Vector3::Dot(corner, axis);
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

    ContainmentType OrientedBoundingBox::Contains(const Vector3& point) const
    {
        return Intersects(point) ? ContainmentType::Contains : ContainmentType::None;
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
        /*constexpr uint AXIS_COUNT = 3;

        // Transform other OBB into local space of this OBB.
        auto rotMat = glm::transpose(Rotation); // Inverse rotation.

        // Get relative position of other OBB's center in this OBB's local space.
        auto relCenter = rotMat * (obb.Center - Center);

        // Check each axis (X, Y, Z) of this OBB and other OBB.
        for (int i = 0; i < AXIS_COUNT; i++)
        {
            // Check if OBBs overlap on this axis (using projection).
            float proj = glm::abs(relCenter[i]);
            if (proj > (Extents[i] + obb.Extents[i]))
            {
                return ContainmentType::Disjoint;
            }
        }

        // Check cross products of axes of both OBBs.
        Vector3 axes[AXIS_COUNT] = { Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f) };
        for (int i = 0; i < AXIS_COUNT; i++)
        {
            for (int j = 0; j < AXIS_COUNT; j++)
            {
                auto axis = glm::cross(axes[i], axes[j]);

                // Calculate projections onto this axis.
                float proj = glm::abs(glm::dot(obb.Extents, axis)) + glm::abs(glm::dot(Extents, axis));
                if (glm::abs(glm::dot(relCenter, axis)) > proj)
                {
                    return ContainmentType::Disjoint;
                }
            }
        }*/

        return ContainmentType::Contains;
    }

    bool OrientedBoundingBox::operator==(const OrientedBoundingBox& obb) const
    {
        return Center == obb.Center && Extents == obb.Extents && Rotation == obb.Rotation;
    }

    bool OrientedBoundingBox::operator!=(const OrientedBoundingBox& obb) const
    {
        return !(*this == obb);
    }
}
