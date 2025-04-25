#pragma once

namespace Silent::Math
{
    class EulerAngles;
    class Quaternion;
    class Vector3;

    class Matrix : public glm::mat4
    {
        // Presets

        static const Matrix Identity;

        // Constructors

        Matrix() = default;

        // Converters

        Vector3     ToDirection() const;
        EulerAngles ToEulerAngles() const;
        Quaternion  ToQuaternion() const;
        AxisAngle   ToAxisAngle() const;
    };
}
