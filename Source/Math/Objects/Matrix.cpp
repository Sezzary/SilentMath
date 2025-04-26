#include "Framework.h"
#include "Math/Objects/Matrix.h"

#include "Math/Objects/AxisAngle.h"
#include "Math/Objects/EulerAngles.h"
#include "Math/Objects/Quaternion.h"
#include "Math/Objects/Vector3.h"

namespace Silent::Math
{
    const Matrix Matrix::Identity = Matrix(1.0f);

    const glm::mat4& Matrix::ToGlmMat4() const
    {
        return *(const glm::mat4*)this;
    }

    glm::mat4& Matrix::ToGlmMat4()
    {
        return *(glm::mat4*)this;
    }

    Vector3 Matrix::ToDirection() const
    {
        return Vector3::Transform(Vector3::UnitZ, *this);
    }

    EulerAngles Matrix::ToEulerAngles() const
    {
        auto quat = ToQuaternion();
        return quat.ToEulerAngles();
    }

    Quaternion Matrix::ToQuaternion() const
    {
        return Quaternion(glm::quat_cast(ToGlmMat4()));
    }

    AxisAngle Matrix::ToAxisAngle() const
    {
        // Compute angle.
        float trace = (*this)[0][0] + (*this)[1][1] + (*this)[2][2];
        float rad = glm::acos((trace - 1.0f) / 2.0f);

        // Compute axis.
        auto axis = Vector3::Zero;
        if (rad == 0.0f)
        {
            axis = Vector3::UnitZ;
        }
        else
        {
            axis = Vector3((*this)[2][1] - (*this)[1][2], 
                           (*this)[0][2] - (*this)[2][0], 
                           (*this)[1][0] - (*this)[0][1]) / (glm::sin(rad) * 2.0f);
        }

        // Set axis and angle.
        return AxisAngle(axis, FP_ANGLE_FROM_RAD(rad));
    }
}
