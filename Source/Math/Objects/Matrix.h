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
        constexpr Matrix(float m)              : glm::mat4(m)   {}
        constexpr Matrix(float m00, float m01, float m02, float m03,
                         float m10, float m11, float m12, float m13,
                         float m20, float m21, float m22, float m23,
                         float m30, float m31, float m32, float m33) :
                         glm::mat4(m00, m01, m02, m03,
                                   m10, m11, m12, m13,
                                   m20, m21, m22, m23,
                                   m30, m31, m32, m33) {}

        // Utilities

        static Matrix Inverse(const Matrix& matrix);
        void          Inverse();

        // Converters

        Vector3 ToTranslation() const;

        Vector3     ToDirection() const;
        EulerAngles ToEulerAngles() const;
        Quaternion  ToQuaternion() const;
        AxisAngle   ToAxisAngle() const;

        Vector3 ToScale() const;

        const glm::mat4& ToGlmMat4() const;
        glm::mat4&       ToGlmMat4();

        // Operators

        bool    operator==(const Matrix& matrix) const;
        bool    operator!=(const Matrix& matrix) const;
        Matrix& operator=(const Matrix& matrix) = default;
        Matrix& operator+=(const Matrix& matrix);
        Matrix& operator-=(const Matrix& matrix);
        Matrix& operator*=(const Matrix& matrix);
        Matrix& operator*=(float scalar);
        Matrix& operator/=(const Matrix& matrix);
        Matrix& operator/=(float scalar);
        Matrix  operator+() const;
        Matrix  operator-() const;
    };
}
