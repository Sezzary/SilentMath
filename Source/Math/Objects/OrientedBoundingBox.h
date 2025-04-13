#pragma once

namespace Silent::Math
{
    typedef class OrientedBoundingBox
    {
    public:
        // Constants

        static constexpr unsigned int CORNER_COUNT = 8;

        // Fields

        glm::vec3 Center   = {};
        glm::vec3 Extents  = {};
        glm::quat Rotation = {};

        // Constructors

       constexpr OrientedBoundingBox() = default;
       //constexpr OrientedBoundingBox(const glm::vec3& center, const glm::vec3& extents, const glm::quat orient) : Center(center), Extents(extents), Orientation(orient) {}

       // Getters

       float                  GetSurfaceArea() const;
       float                  GetVolume() const;
       glm::mat4              GetTransformMatrix() const;
       std::vector<glm::vec3> GetCorners() const;

       // Utilities

    } Obb;
}
