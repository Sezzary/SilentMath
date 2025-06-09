#include "Framework.h"
#include "Math/Objects/Matrix.h"

#include "Math/Objects/AxisAngle.h"
#include "Math/Objects/EulerAngles.h"
#include "Math/Objects/Quaternion.h"
#include "Math/Objects/Vector3.h"

namespace Silent::Math
{
    const Matrix Matrix::Identity = Matrix(1.0f);

    Matrix Matrix::Inverse(const Matrix& mat)
    {
        return Matrix(glm::inverse(mat.ToGlmMat4()));
    }

    void Matrix::Inverse()
    {
        *this = Matrix::Inverse(*this);
    }

    Matrix Matrix::Translate(const Matrix& mat, const Vector3& offset)
    {
        return glm::translate(mat, offset);
    }

    void Matrix::Translate(const Vector3& offset)
    {
        *this = Matrix::Translate(*this, offset);
    }

    Matrix Matrix::Rotate(const Matrix& mat, float rad, const Vector3& axis)
    {
       return glm::rotate(mat.ToGlmMat4(), rad, axis);
    }

    void Matrix::Rotate(float rad, const Vector3& axis)
    {
        *this = Matrix::Rotate(*this, rad, axis);
    }

    Matrix Matrix::Scale(const Matrix& mat, const Vector3& scale)
    {
        return glm::scale(mat.ToGlmMat4(), scale.ToGlmVec3());
    }

    void Matrix::Scale(const Vector3& scale)
    {
        *this = Matrix::Scale(*this, scale);
    }

    const glm::mat4& Matrix::ToGlmMat4() const
    {
        return *(const glm::mat4*)this;
    }

    glm::mat4& Matrix::ToGlmMat4()
    {
        return *(glm::mat4*)this;
    }

    Vector3 Matrix::ToTranslation() const
    {
        return Vector3((*this)[3][0], (*this)[3][1], (*this)[3][2]);
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
        float rad   = glm::cos((trace - 1.0f) / 2.0f);

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

    Vector3 Matrix::ToScale() const
    {
        return Vector3(Vector3((*this)[0][0], (*this)[0][1], (*this)[0][2]).Length(),
                       Vector3((*this)[1][0], (*this)[1][1], (*this)[1][2]).Length(),
                       Vector3((*this)[2][0], (*this)[2][1], (*this)[2][2]).Length());
    }

    bool Matrix::operator==(const Matrix& mat) const
    {
        return ToGlmMat4() == mat.ToGlmMat4();
    }

    bool Matrix::operator!=(const Matrix& mat) const
    {
        return ToGlmMat4() != mat.ToGlmMat4();
    }

    Matrix& Matrix::operator+=(const Matrix& mat)
    {
        ToGlmMat4() += mat.ToGlmMat4();
        return *this;
    }

    Matrix& Matrix::operator-=(const Matrix& mat)
    {
        ToGlmMat4() -= mat.ToGlmMat4();
        return *this;
    }

    Matrix& Matrix::operator*=(const Matrix& mat)
    {
        ToGlmMat4() *= mat.ToGlmMat4();
        return *this;
    }

    Matrix& Matrix::operator*=(float scalar)
    {
        ToGlmMat4() *= scalar;
        return *this;
    }

    Matrix& Matrix::operator/=(const Matrix& mat)
    {
        ToGlmMat4() /= mat.ToGlmMat4();
        return *this;
    }

    Matrix& Matrix::operator/=(float scalar)
    {
        ToGlmMat4() /= scalar;
        return *this;
    }
}
