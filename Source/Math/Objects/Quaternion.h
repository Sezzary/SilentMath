#pragma once

namespace Silent::Math
{
    class AxisAngle;
    class EulerAngles;
    class Matrix;
    class Vector3;

    class Quaternion : public glm::quat
    {
    public:
        // Presets

        static const Quaternion Identity;

        // Constructors

        constexpr Quaternion() = default;
        constexpr Quaternion(const glm::quat& quat) : glm::quat(quat) {}
        constexpr Quaternion(float x, float y, float z, float w)      { this->x = x; this->y = y; this->z = z; this->w = w; }
        constexpr Quaternion(const Vector3& vec, float scalar)        { this->x = vec.x; this->y = vec.y; this->z = vec.z; this->w = scalar; }
                  Quaternion(const Vector3& dir);

        // Utilities

        static Quaternion Lerp(const Quaternion& from, const Quaternion& to, float alpha);
        void              Lerp(const Quaternion& to, float alpha);
        static Quaternion Slerp(const Quaternion& from, const Quaternion& to, float alpha);
        void              Slerp(const Quaternion& to, float alpha);

        // Converters

        Vector3     ToDirection() const;
        EulerAngles ToEulerAngles() const;
        AxisAngle   ToAxisAngle() const;
        Matrix      ToMatrix() const;

        const glm::quat& ToGlmQuat() const;
        glm::quat&       ToGlmQuat();

        // Operators

        bool        operator==(const Quaternion& quat) const;
        bool        operator!=(const Quaternion& quat) const;
        Quaternion& operator=(const Quaternion& quat) = default;
        Quaternion& operator+=(const Quaternion& quat);
        Quaternion& operator-=(const Quaternion& quat);
        Quaternion& operator*=(const Quaternion& quat);
        Quaternion& operator*=(float scalar);
        Quaternion& operator/=(const Quaternion& quat);
        Quaternion  operator+(const Quaternion& quat) const;
        Quaternion  operator-(const Quaternion& quat) const;
        Quaternion  operator*(const Quaternion& quat) const;
        Quaternion  operator*(float scalar) const;
        Quaternion  operator/(const Quaternion& quat) const;
    };
}
