#include "Framework.h"
#include "Math/Objects/Matrix.h"

#include "Math/Objects/AxisAngle.h"
#include "Math/Objects/EulerAngles.h"
#include "Math/Objects/Quaternion.h"

namespace Silent::Math
{
    const glm::mat4& Matrix::ToGlmMat4() const
    {
        return *(const glm::mat4*)this;
    }

    glm::mat4& Matrix::ToGlmMat4()
    {
        return *(glm::mat4*)this;
    }

    Quaternion Matrix::ToQuaternion() const
    {
        return Quaternion(glm::quat_cast(ToGlmMat4()));
    }
}
