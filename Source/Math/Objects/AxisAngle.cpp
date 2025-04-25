#include "Framework.h"
#include "Math/Objects/AxisAngle.h"

#include "Math/Constants.h"
#include "Math/Objects/EulerAngles.h"
#include "Math/Objects/Matrix.h"

namespace Silent::Math
{
    const AxisAngle AxisAngle::Identity = AxisAngle(glm::vec3(), 0);

    AxisAngle::AxisAngle(const Vector3& dir)
    {
        // Compute axis.
        auto axis = Vector3::Normalize(Vector3::Cross(Vector3::UnitZ, dir));
        if (axis.LengthSquared() < EPSILON)
        {
            axis = Vector3::UnitZ;
        }

        // Compute angle.
        float dotProduct = Vector3::Dot(dir, Vector3::UnitZ);
        float rad = glm::acos(dotProduct);

        // Set axis angl angle.
        Axis = axis;
        Angle = FP_ANGLE_FROM_RAD(rad);
    }

    AxisAngle::AxisAngle(const EulerAngles& eulerAngles)
    {
        auto quat = eulerAngles.ToQuaternion();
        *this = AxisAngle(quat);
    }

    AxisAngle::AxisAngle(const glm::quat& quat)
    {
        float sinHalfAngle = glm::sqrt(1.0f - SQUARE(quat.w));

        // Compute axis.
        auto axis = glm::vec3();
        if (sinHalfAngle < EPSILON)
        {
            axis = Vector3::UnitZ;
        }
        else
        {
            axis = Vector3(quat.x, quat.y, quat.z) / sinHalfAngle;
        }

        // Compute angle.
        float rad = glm::acos(quat.w) * 2.0f;

        Axis = axis;
        Angle = FP_ANGLE_FROM_RAD(rad);
    }

    AxisAngle::AxisAngle(const Matrix& mat)
    {
        // Compute angle.
        float trace = mat[0][0] + mat[1][1] + mat[2][2];
        float rad = glm::acos((trace - 1.0f) / 2.0f);

        // Compute axis.
        auto axis = Vector3::Zero;
        if (rad == 0.0f)
        {
            axis = Vector3::UnitZ;
        }
        else
        {
            axis = Vector3(mat[2][1] - mat[1][2], 
                           mat[0][2] - mat[2][0], 
                           mat[1][0] - mat[0][1]) / (glm::sin(rad) * 2.0f);
        }

        // Set axis and angle.
        Axis = axis;
        Angle = FP_ANGLE_FROM_RAD(rad);
    }

    void AxisAngle::Slerp(const AxisAngle& to, float alpha)
    {
        *this = Slerp(*this, to, alpha);
    }

    AxisAngle AxisAngle::Slerp(const AxisAngle& from, const AxisAngle& to, float alpha)
    {
        auto quatFrom = from.ToQuaternion();
        auto quatTo = to.ToQuaternion();
        auto quat = glm::slerp(quatFrom, quatTo, alpha);
        return AxisAngle(quat);
    }

    Vector3 AxisAngle::ToDirection() const
    {
        float rad = FP_ANGLE_TO_RAD(Angle);
        return (glm::cos(rad) * Axis) + ((1.0f - glm::cos(rad)) * Axis);
    }

    EulerAngles AxisAngle::ToEulerAngles() const
    {
        return EulerAngles(*this);
    }

    glm::quat AxisAngle::ToQuaternion() const
    {
        float rad = FP_ANGLE_TO_RAD(Angle);

        float halfAngle = rad / 2.0f;
        float sinHalfAngle = glm::sin(halfAngle);
        float cosHalfAngle = glm::cos(halfAngle);

        return glm::quat(cosHalfAngle, Axis.x * sinHalfAngle, Axis.y * sinHalfAngle, Axis.z * sinHalfAngle);
    }

    Matrix AxisAngle::ToMatrix() const
    {
        float rad = FP_ANGLE_TO_RAD(Angle);
        float sinAngle = glm::sin(rad);
        float cosAngle = glm::cos(rad);
        float oneMinusCos = 1.0f - cosAngle;
    
        // Construct rotation matrix using Rodrigues' formula.
        return Matrix(glm::mat3
        {
            {
                cosAngle + SQUARE(Axis.x) * oneMinusCos,
                ((Axis.x * Axis.y) * oneMinusCos) - (Axis.z * sinAngle),
                ((Axis.x * Axis.y) * oneMinusCos) + (Axis.z * sinAngle),
            },
            {
                ((Axis.x * Axis.y) * oneMinusCos) + (Axis.z * sinAngle),
                cosAngle + (SQUARE(Axis.y) * oneMinusCos),
                ((Axis.y * Axis.z) * oneMinusCos) - (Axis.x * sinAngle)
            },
            {
                ((Axis.x * Axis.z) * oneMinusCos) - (Axis.y * sinAngle),
                ((Axis.y * Axis.z) * oneMinusCos) + (Axis.x * sinAngle),
                cosAngle + (SQUARE(Axis.z) * oneMinusCos)
            }
        });
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
        return AxisAngle(quat);
    }
}
