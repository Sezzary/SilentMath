#pragma once

#include "Math/Constants.h"
#include "Math/Objects/Vector3.h"

namespace Silent::Math
{
    class EulerAngles;
    class Matrix;
    class Quaternion;

    class AxisAngle
    {
    public:
        // Fields

        Vector3 Axis  = Vector3::UnitZ;
        short   Angle = 0;

        // Presets

        static const AxisAngle Identity;

        // Constructors

        constexpr AxisAngle() = default;
        constexpr AxisAngle(const Vector3& axis, short angle) : Axis(axis), Angle(angle) {};
                  AxisAngle(const Vector3& dir);

        // Utilities

        void             Slerp(const AxisAngle& to, float alpha);
        static AxisAngle Slerp(const AxisAngle& from, const AxisAngle& to, float alpha);

        // Converters

        Vector3     ToDirection() const;
        EulerAngles ToEulerAngles() const;
        Quaternion  ToQuaternion() const;
        Matrix      ToMatrix() const;

        // Operators

        bool       operator==(const AxisAngle& axisAngle) const;
        bool       operator!=(const AxisAngle& axisAngle) const;
        AxisAngle& operator=(const AxisAngle& axisAngle);
        AxisAngle& operator*=(const AxisAngle& axisAngle);
        AxisAngle  operator*(const AxisAngle& axisAngle) const;
    };
}
