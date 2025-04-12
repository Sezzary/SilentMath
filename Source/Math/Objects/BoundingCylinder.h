#pragma once

namespace Silent::Math
{
    class BoundingCylinder
    {
    public:
        // Fields

        glm::vec3 Center      = {};
        float     Radius      = 0.0f;
        float     Height      = 0.0f;
        glm::quat Orientation = {};

        // Constructors

        constexpr BoundingCylinder() = default;
        //constexpr BoundingCylinder(const glm::vec3& center, float radius, float height, const glm::quat& orient) : Center(center), Radius(radius), Height(height), Orientation(orient) {}

        // Getters

        float GetSurfaceArea() const;
        float GetVolume() const;
    };
}
