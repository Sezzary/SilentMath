#pragma once

#include "Math/Objects/Vector3.h"
#include "Math/Objects/Vector4.h"

namespace Silent::Math
{
    class AxisAngle;
    class EulerAngles;
    class Matrix;

    /** @brief Quaternion rotation representation. */
    class Quaternion : public glm::quat
    {
    public:
        // ========
        // Presets
        // ========

        static const Quaternion Identity;

        // =============
        // Constructors
        // =============

        /** @brief Creates a default identity instance. */
        constexpr Quaternion() : glm::quat(1.0f, 0.0f, 0.0f, 0.0f) {}

        /** @brief Creates an instance from XYZW components.
         *
         * @param x X component.
         * @param y Y component.
         * @param z Z component.
         * @param w W component.
         */
        constexpr Quaternion(float x, float y, float z, float w) : glm::quat(w, x, y, z) {}

        /** @brief Creates an instance from a `glm::quat`.
         *
         * @param quat GLM quaternion.
         */
        constexpr Quaternion(const glm::quat& quat) : glm::quat(quat) {}

        /** @brief Creates an instance from a `Vector4`.
         *
         * @param vec XYZW vector.
         */
        constexpr Quaternion(const Vector4& vec) : glm::quat(vec.ToGlmVec4()) {}

        /** @brief Creates an instance from a `Vector3` and a scalar.
         *
         * @param vec XYZ vector.
         * @param scalar W scalar.
         */
        constexpr Quaternion(const Vector3& vec, float scalar) : glm::quat(scalar, vec.ToGlmVec3()) {}

        /** @brief Creates an instance from a direction vector.
         *
         * @param dir Direction vector.
         */
        Quaternion(const Vector3& dir);

        // ==========
        // Utilities
        // ==========

        /** @brief Computes the angular distance between two quaternions.
         *
         * @param from Start quaternion.
         * @param to End quaternion.
         * @return Angular distance in randians.
         */
        static float AngularDistance(const Quaternion& from, const Quaternion& to);

        /** @brief Inverts a quaternion.
         *
         * @param quat Quaternion to invert.
         * @return Inverted quaternion.
         */
        static Quaternion Invert(const Quaternion& quat);

        /** @brief Inverts the quaternion. */
        void Invert();

        /** @brief Linearly interpolates between two quaternions.
         *
         * @param from Start quaternion.
         * @param to End quaternion.
         * @param alpha Interpolation alpha.
         * @return Interpolated quaternion.
         */
        static Quaternion Lerp(const Quaternion& from, const Quaternion& to, float alpha);

        /** @brief @brief Linearly interpolates between the quaternion and another quaternion.
         *
         * @param to End quaternion.
         */
        void Lerp(const Quaternion& to, float alpha);

        /** @brief Computes the spherical linear interpolation between two quaternions.
         *
         * @param from Start quaternion.
         * @param to End quaternion
         * @param alpha Interpolation alpha.
         * @return Interpolated quaternion.
         */
        static Quaternion Slerp(const Quaternion& from, const Quaternion& to, float alpha);

        /** @brief Computes the spherical linear interpolation between the quaternion and another quaternion.
         *
         * @param to End quaternion
         * @param alpha Interpolation alpha.
         */
        void Slerp(const Quaternion& to, float alpha);

        // ===========
        // Converters
        // ===========

        /** @brief Converts the quaternion to a normalized direction vector.
         *
         * @return Normalized direction vector.
         */
        Vector3 ToDirection() const;

        /** @brief Converts the quaternion to a set of Euler angles.
         *
         * @return Euler angles rotation.
         */
        EulerAngles ToEulerAngles() const;

        /** @brief Converts the quaternion to an axis angle.
         *
         * @return Axis angle rotation.
         */
        AxisAngle ToAxisAngle() const;

        /** @brief Converts the quaternion to a rotation matrix.
         *
         * @return Rotation matrix.
         */
        Matrix ToRotationMatrix() const;

        /** @brief Downcasts the quaternion to a read-only `glm::quat`.
         *
         * @return Read-only `glm::quat`.
         */
        const glm::quat& ToGlmQuat() const;

        /** @brief Downcasts the quaternion to a writable `glm::quat`.
         *
         * @return Writable `glm::quat`.
         */
        glm::quat& ToGlmQuat();

        // ==========
        // Operators
        // ==========

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
