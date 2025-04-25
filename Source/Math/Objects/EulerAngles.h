#pragma once

namespace Silent::Math
{
    class AxisAngle;

    class EulerAngles
    {
    private:
        // Constants

        static constexpr auto DEFAULT_EPSILON = (short)3;

    public:
        // Fields

        short x = 0; // Pitch
        short y = 0; // Yaw
        short z = 0; // Roll

        // Constructors

        constexpr EulerAngles() = default;
        constexpr EulerAngles(short x, short y, short z) { this->x = x; this->y = y; this->z = z; }
                  EulerAngles(const glm::vec3& dir);
                  EulerAngles(const AxisAngle& axisAngle);
                  EulerAngles(const glm::quat& quat);
                  EulerAngles(const glm::mat3& rotMat);

        // Utilities

        static bool        Compare(const EulerAngles& eulerAngles0, const EulerAngles& eulerAngles1, short epsilon = DEFAULT_EPSILON);
        void               Lerp(const EulerAngles& to, float alpha, short epsilon = DEFAULT_EPSILON);
        static EulerAngles Lerp(const EulerAngles& from, const EulerAngles& to, float alpha, short epsilon = DEFAULT_EPSILON);
        void               Slerp(const EulerAngles& to, float alpha);
        static EulerAngles Slerp(const EulerAngles& from, const EulerAngles& to, float alpha);
        void               InterpolateConstant(const EulerAngles& to, short angularVel);
        static EulerAngles InterpolateConstant(const EulerAngles& from, const EulerAngles& eulerTo, short angularVel);

        // Converters

        glm::vec3 ToDirection() const;
        glm::quat ToQuaternion() const;
        AxisAngle ToAxisAngle() const;
        glm::mat3 ToRotationMatrix() const;

        // Operators

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
        // Helpers

        static bool    Compare(short angle0, short angle1, short epsilon = DEFAULT_EPSILON);
        static short Lerp(short from, short to, float alpha, short epsilon = DEFAULT_EPSILON);
        static short InterpConstant(short from, short to, short angularVel);
    };
}
