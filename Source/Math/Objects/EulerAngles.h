#pragma once

namespace Silent::Math
{
    class AxisAngle;
    class Matrix;
    class Quaternion;
    class Vector3;

    /** @brief Euler angles rotation representation.
     *
     * @note Angle components are in fixed-point Q1.15 format.
     */
    class EulerAngles
    {
    private:
        // ==========
        // Constants
        // ==========

        static constexpr short DEFAULT_EPSILON = 3;

    public:
        // =======
        // Fields
        // =======

        short x = 0; /** Pitch. */
        short y = 0; /** Yaw. */
        short z = 0; /** Roll. */

        // ========
        // Presets
        // ========

        static const EulerAngles Identity;

        // =============
        // Constructors
        // =============

        /** @brief Constructs a default `EulerAngles`. */
        constexpr EulerAngles() = default;

        /** @brief Constructs an `EulerAngles` from XYZ angle components.
         *
         * @param x X angle component.
         * @param y Y angle component.
         * @param z Z angle component.
         */
        constexpr EulerAngles(short x, short y, short z) : x(x), y(y), z(z) {}

        /** @brief Constructs an `EulerAngles` from a direction vector.
         *
         * @param dir Direction vector.
         */
        EulerAngles(const Vector3& dir);

        // ==========
        // Utilities
        // ==========

        /** @brief Computes a constant interpolation between two sets of Euler angles.
         *
         * @param from Start Euler angles.
         * @param to End Euler angles.
         * @param angularVel Angular velocity.
         */
        static EulerAngles InterpConstant(const EulerAngles& from, const EulerAngles& to, short angularVel);

        /** @brief Computes a constant interpolation between the set of Euler angles and another set of Euler angles.
         *
         * @param to End Euler angles.
         * @param angularVel Angular velocity.
         */
        void InterpConstant(const EulerAngles& to, short angularVel);

        /** @brief Linearly interpolates between two sets of Euler angles.
         *
         * @param from Start Euler angles.
         * @param to End Euler angles.
         * @param alpha Interpolation alpha.
         * @param epsilon Epsilon angle.
         * @return Interpolated set of Euler angles.
         */
        static EulerAngles Lerp(const EulerAngles& from, const EulerAngles& to, float alpha, short epsilon = DEFAULT_EPSILON);

        /** @brief Linearly interpolates between the set of Euler angles and another set of Euler angles.
         *
         * @param to End Euler angles.
         * @param alpha Interpolation alpha.
         * @param epsilon Epsilon angle.
         */
        void Lerp(const EulerAngles& to, float alpha, short epsilon = DEFAULT_EPSILON);

        /** @brief Computes the spherical linear interpolation between two sets of Euler angles.
         *
         * @param from Start Euler angles.
         * @param to End Euler angles.
         * @param alpha Interpolation alpha.
         * @return Interpolated set of Euler angles.
         */
        static EulerAngles Slerp(const EulerAngles& from, const EulerAngles& to, float alpha);

        /** @brief Computes the spherical linear interpolation between the set of Euler angles and another set of Euler angles.
         *
         * @param to End Euler angles.
         * @param alpha Interpolation alpha.
         */
        void Slerp(const EulerAngles& to, float alpha);

        /** @brief Compares two sets of Euler angles.
         *
         * @param eulerAngles0 First set of Euler angles.
         * @param eulerAngles1 Second set of Euler angles.
         * @param epsilon Epsilon angle.
         * @return `true` if the sets of Euler angles are within the difference of the epsilon, `false` otherwise.
         */
        static bool Compare(const EulerAngles& eulerAngles0, const EulerAngles& eulerAngles1, short epsilon = DEFAULT_EPSILON);

        // ===========
        // Converters
        // ===========

        /** @brief Converts the set of Euler angles to a normalized direction vector.
         *
         * @return Normalized direction vector.
         */
        Vector3 ToDirection() const;

        /** @brief Converts the set of Euler angles to a quaternion.
         *
         * @return Quaternion rotation.
         */
        Quaternion ToQuaternion() const;

        /** @brief Converts the set of Euler angles to an axis-angle.
         *
         * @return Axis angle rotation.
         */
        AxisAngle ToAxisAngle() const;

        /** @brief Converts the set of Euler angles to a rotation matrix.
         *
         * @return Rotation matrix.
         */
        Matrix ToRotationMatrix() const;

        // ==========
        // Operators
        // ==========

        bool         operator==(const EulerAngles& eulerAngles) const;
        bool         operator!=(const EulerAngles& eulerAngles) const;
        EulerAngles& operator=(const EulerAngles& eulerAngles);
        EulerAngles& operator+=(const EulerAngles& eulerAngles);
        EulerAngles& operator-=(const EulerAngles& eulerAngles);
        EulerAngles& operator*=(const EulerAngles& eulerAngles);
        EulerAngles& operator*=(float scalar);
        EulerAngles& operator/=(float scalar);
        EulerAngles  operator+(const EulerAngles& eulerAngles) const;
        EulerAngles  operator-(const EulerAngles& eulerAngles) const;
        EulerAngles  operator*(const EulerAngles& eulerAngles) const;
        EulerAngles  operator*(float scalar) const;
        EulerAngles  operator/(float scalar) const;

    private:
        // ========
        // Helpers
        // ========

        /** @brief Computes a linear interpolation between two angles.
         *
         * @param from Start angle.
         * @param to End angle.
         * @param angularVel Angular velocity.
         */
        static short InterpConstant(short from, short to, short angularVel);

        /** @brief Linearly interpolates between two angles.
         *
         * @param from Start angle.
         * @param to End angle.
         * @param alpha Interpolation alpha.
         * @param epsilon Epsilon angle.
         */
        static short Lerp(short from, short to, float alpha, short epsilon = DEFAULT_EPSILON);

        /** @brief Compares two angles.
         *
         * @param angle0 First angle.
         * @param angle1 Second angle.
         * @param epsilon Epsilon angle.
         * @return `true` if the angles are within the difference of the epsilon, `false` otherwise.
         */
        static bool Compare(short angle0, short angle1, short epsilon = DEFAULT_EPSILON);
    };
}
