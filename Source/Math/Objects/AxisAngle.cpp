#include "Framework.h"
#include "Math/Objects/AxisAngle.h"

#include "Math/Constants.h"
#include "Math/Objects/EulerAngles.h"
#include "Math/Objects/Matrix.h"
#include "Math/Objects/Quaternion.h"
#include "Math/Objects/Vector3.h"

namespace Silent::Math
{
    const AxisAngle AxisAngle::Identity = AxisAngle(Vector3::UnitZ, 0);

    AxisAngle::AxisAngle(const Vector3& dir)
    {
        // Compute axis.
        auto axis = Vector3::Normalize(Vector3::Cross(Vector3::UnitZ, dir));
        if (axis.LengthSquared() < EPSILON)
        {
            axis = Vector3::UnitZ;
        }

        // Compute angle.
        float dot = Vector3::Dot(dir, Vector3::UnitZ);
        float rad = acos(dot);

        // Set axis angle.
        Axis = axis;
        Angle = FP_ANGLE_FROM_RAD(rad);
    }

    AxisAngle AxisAngle::Lerp(const AxisAngle& from, const AxisAngle& to, float alpha)
    {
        auto quatFrom = from.ToQuaternion();
        auto quatTo = to.ToQuaternion();

        auto quat = Quaternion::Lerp(quatFrom, quatTo, alpha);
        return quat.ToAxisAngle();
    }

    void AxisAngle::Lerp(const AxisAngle& to, float alpha)
    {
        *this = Lerp(*this, to, alpha);
    }

    AxisAngle AxisAngle::Slerp(const AxisAngle& from, const AxisAngle& to, float alpha)
    {
        auto quatFrom = from.ToQuaternion();
        auto quatTo = to.ToQuaternion();

        auto quat = Quaternion::Slerp(quatFrom, quatTo, alpha);
        return quat.ToAxisAngle();
    }

    void AxisAngle::Slerp(const AxisAngle& to, float alpha)
    {
        *this = Slerp(*this, to, alpha);
    }

    Vector3 AxisAngle::ToDirection() const
    {
        float rad = FP_ANGLE_TO_RAD(Angle);
        return (Axis * cos(rad)) + (Axis * (1.0f - cos(rad)));
    }

    EulerAngles AxisAngle::ToEulerAngles() const
    {
        auto quat = ToQuaternion();
        return quat.ToEulerAngles();
    }

    Quaternion AxisAngle::ToQuaternion() const
    {
        float rad = FP_ANGLE_TO_RAD(Angle);

        float halfAngle = rad / 2.0f;
        float sinHalfAngle = sin(halfAngle);
        float cosHalfAngle = cos(halfAngle);

        return Quaternion(Axis.x * sinHalfAngle,
                          Axis.y * sinHalfAngle,
                          Axis.z * sinHalfAngle,
                          cosHalfAngle);
    }

    Matrix AxisAngle::ToRotationMatrix() const
    {
        float rad = FP_ANGLE_TO_RAD(Angle);
        float sinAngle = sin(rad);
        float cosAngle = cos(rad);
        float oneMinusCos = 1.0f - cosAngle;
    
        return Matrix(cosAngle + SQUARE(Axis.x) * oneMinusCos,
                      ((Axis.x * Axis.y) * oneMinusCos) - (Axis.z * sinAngle),
                      ((Axis.x * Axis.y) * oneMinusCos) + (Axis.z * sinAngle),
                      0.0f,

                      ((Axis.x * Axis.y) * oneMinusCos) + (Axis.z * sinAngle),
                      cosAngle + (SQUARE(Axis.y) * oneMinusCos),
                      ((Axis.y * Axis.z) * oneMinusCos) - (Axis.x * sinAngle),
                      0.0f,

                      ((Axis.x * Axis.z) * oneMinusCos) - (Axis.y * sinAngle),
                      ((Axis.y * Axis.z) * oneMinusCos) + (Axis.x * sinAngle),
                      cosAngle + (SQUARE(Axis.z) * oneMinusCos),
                      0.0f,

                      0.0f,
                      0.0f,
                      0.0f,
                      1.0f);
    }

    bool AxisAngle::operator==(const AxisAngle& axisAngle) const
    {
        return Axis == axisAngle.Axis && Angle == axisAngle.Angle;
    }

    bool AxisAngle::operator!=(const AxisAngle& axisAngle) const
    {
        return !(*this == axisAngle);
    }
    
    AxisAngle& AxisAngle::operator=(const AxisAngle& axisAngle)
    {
        Axis = axisAngle.Axis;
        Angle = axisAngle.Angle;
        return *this;
    }
    
    AxisAngle& AxisAngle::operator*=(const AxisAngle& axisAngle)
    {
        *this = *this * axisAngle;
        return *this;
    }
    
    AxisAngle AxisAngle::operator*(const AxisAngle& axisAngle) const
    {
        auto quat0 = ToQuaternion();
        auto quat1 = axisAngle.ToQuaternion();

        auto quat = quat0 * quat1;
        return quat.ToAxisAngle();
    }
}
