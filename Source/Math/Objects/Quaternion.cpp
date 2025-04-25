#include "Framework.h"
#include "Math/Objects/Quaternion.h"

#include "Math/Constants.h"
#include "Math/Objects/Vector3.h"

namespace Silent::Math
{
    Quaternion Quaternion::Lerp(const Quaternion& from, const Quaternion& to, float alpha)
    {
        return Quaternion(glm::lerp(from.ToGlmQuat(), to.ToGlmQuat(), alpha));
    }

    void Quaternion::Lerp(const Quaternion& to, float alpha)
    {
        *this = Quaternion::Lerp(*this, to, alpha);
    }

    Quaternion Quaternion::Slerp(const Quaternion& from, const Quaternion& to, float alpha)
    {
        return Quaternion(glm::slerp(from.ToGlmQuat(), to.ToGlmQuat(), alpha));
    }

    void Quaternion::Slerp(const Quaternion& to, float alpha)
    {
        *this = Quaternion::Slerp(*this, to, alpha);
    }

    /*Vector3 Quaternion::ToDirection() const
    {
        return Vector3(glm::rotate(ToGlmQuat(), glm::vec3(0.0f, 0.0f, 1.0f)));
    }*/

    EulerAngles Quaternion::ToEulerAngles() const
    {
        auto eulerAnglesRad = glm::eulerAngles(ToGlmQuat());
        return EulerAngles(FP_ANGLE_FROM_RAD(eulerAnglesRad.x), FP_ANGLE_FROM_RAD(eulerAnglesRad.y), FP_ANGLE_FROM_RAD(eulerAnglesRad.z));
    }

    const glm::quat& Quaternion::ToGlmQuat() const
    {
        return *(const glm::quat*)this;
    }

    glm::quat& Quaternion::ToGlmQuat()
    {
        return *(glm::quat*)this;
    }
}
