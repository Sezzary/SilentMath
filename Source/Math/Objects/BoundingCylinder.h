#pragma once

namespace Silent::Math
{
    class BoundingCylinder
    {
    public:
        // Fields

        glm::vec3 Center      = glm::vec3();
        glm::quat Orientation = glm::quat();
        float     Radius      = 0.0f;
        float     Height      = 0.0f;

        // Constructors

        constexpr BoundingCylinder() = default;
        //constexpr BoundingCylinder(const glm::vec3& center, float radius, float height, const glm::quat& orient) : Center(center), Radius(radius), Height(height), Orientation(orient) {}

        // Getters

        float GetSurfaceArea() const;
        float GetVolume() const;
    };
}
