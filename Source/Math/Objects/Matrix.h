#pragma once

namespace Silent::Math
{
    class EulerAngles;
    class Quaternion;
    class Vector3;

    class Matrix : public glm::mat4
    {
    public:
        // Presets

        static const Matrix Identity;

        // Constructors

        constexpr Matrix() = default;
        constexpr Matrix(const glm::mat4& mat) : glm::mat4(mat) {}
        constexpr Matrix(float x0, float y0, float z0,
                         float x1, float y1, float z1,
                         float x2, float y2, float z2,
                         float x3, float y3, float z3) {} // TODO

        // Utilities

        // Converters

        Vector3     ToDirection() const;
        EulerAngles ToEulerAngles() const;
        Quaternion  ToQuaternion() const;
        AxisAngle   ToAxisAngle() const;

        const glm::mat4& ToGlmMat4() const;
        glm::mat4&       ToGlmMat4();
    };
}
