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


        // Utilities

        // Converters

        const glm::mat4& ToGlmMat4() const;
        glm::mat4&       ToGlmMat4();

        Vector3     ToDirection() const;
        EulerAngles ToEulerAngles() const;
        Quaternion  ToQuaternion() const;
        AxisAngle   ToAxisAngle() const;
    };
}
