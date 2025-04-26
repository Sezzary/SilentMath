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
        constexpr Matrix(float m00, float m01, float m02, float m03,
                         float m10, float m11, float m12, float m13,
                         float m20, float m21, float m22, float m23,
                         float m30, float m31, float m32, float m33) :
                         glm::mat4(m00, m01, m02, m03,
                                   m10, m11, m12, m13,
                                   m20, m21, m22, m23,
                                   m30, m31, m32, m33) {}

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
