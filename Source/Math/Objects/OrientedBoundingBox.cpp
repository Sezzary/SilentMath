#include "Framework.h"
#include "Math/Objects/OrientedBoundingBox.h"

#include "Math/Constants.h"

namespace Silent::Math
{
    float OrientedBoundingBox::GetVolume() const
    {
        return Extents.x * Extents.y * Extents.z;
    }

    float OrientedBoundingBox::GetSurfaceArea() const
    {
        return ((Extents.x * Extents.y) + (Extents.x * Extents.z) + (Extents.y * Extents.z)) * 2.0f;
    }

    glm::mat4 OrientedBoundingBox::GetTransformMatrix() const
    {
        auto translationMat = glm::translate(glm::mat4(1.0f), Center);
        auto rotMat = glm::mat4_cast(Rotation);
    
        return rotMat * translationMat;
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
}
