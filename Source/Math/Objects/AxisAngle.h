#pragma once

namespace Silent::Math
{
    class EulerAngles;

    class AxisAngle
    {
    private:
        // Constants

        static constexpr auto DEFAULT_AXIS = glm::vec3(0.0f, 0.0f, 1.0f);

        // Fields

        glm::vec3 Axis  = DEFAULT_AXIS;
        short     Angle = 0;

    public:
        // Constructors

        constexpr AxisAngle() = default;
        constexpr AxisAngle(const glm::vec3& axis, short angle) : Axis(axis), Angle(angle) {};
                  AxisAngle(const glm::vec3& dir);
                  AxisAngle(const EulerAngles& eulerAngles);
                  AxisAngle(const glm::quat& quat);
                  AxisAngle(const glm::mat3& rotMatrix);

        // Utilities

        void             Slerp(const AxisAngle& to, float alpha);
        static AxisAngle Slerp(const AxisAngle& from, const AxisAngle& to, float alpha);

        // Converters

        glm::vec3   ToDirection() const;
        EulerAngles ToEulerAngles() const;
        glm::quat   ToQuaternion() const;
        glm::mat3   ToRotationMatrix() const;

        // Operators

        bool       operator ==(const AxisAngle& axisAngle) const;
        bool       operator !=(const AxisAngle& axisAngle) const;
        AxisAngle& operator =(const AxisAngle& axisAngle);
        AxisAngle& operator *=(const AxisAngle& axisAngle);
        AxisAngle  operator *(const AxisAngle& axisAngle) const;
    };
}
