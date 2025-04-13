#include "Framework.h"
#include "Math/Objects/EulerAngles.h"

#include "Math/Constants.h"
#include "Math/Objects/AxisAngle.h"

namespace Silent::Math
{
    EulerAngles::EulerAngles(const glm::vec3& dir)
    {
        auto dirNorm = glm::normalize(dir);

        x = FP_ANGLE_FROM_RAD(-glm::asin(dirNorm.y));
        y = FP_ANGLE_FROM_RAD(glm::atan2(dirNorm.x, dirNorm.z));
        z = FP_ANGLE_FROM_RAD(0.0f);
    }

    EulerAngles::EulerAngles(const AxisAngle& axisAngle)
    {
        auto quat = ToQuaternion();
        *this = EulerAngles(quat);
    }

    EulerAngles::EulerAngles(const glm::quat& quat)
    {
        auto eulerAnglesRad = glm::eulerAngles(quat);
        *this = EulerAngles(FP_ANGLE_FROM_RAD(eulerAnglesRad.x), FP_ANGLE_FROM_RAD(eulerAnglesRad.y), FP_ANGLE_FROM_RAD(eulerAnglesRad.z));
    }

    EulerAngles::EulerAngles(const glm::mat3& rotMat)
    {
        auto quat = glm::quat_cast(rotMat);
        auto eulerAnglesRad = glm::eulerAngles(quat);
        *this = EulerAngles(FP_ANGLE_FROM_RAD(eulerAnglesRad.x), FP_ANGLE_FROM_RAD(eulerAnglesRad.y), FP_ANGLE_FROM_RAD(eulerAnglesRad.z));
    }

    bool EulerAngles::Compare(const EulerAngles& eulerAngles0, const EulerAngles& eulerAngles1, short epsilon)
    {
        return Compare(eulerAngles0.x, eulerAngles1.x, epsilon) &&
               Compare(eulerAngles0.y, eulerAngles1.y, epsilon) &&
               Compare(eulerAngles0.z, eulerAngles1.z, epsilon);
    }

    void EulerAngles::Lerp(const EulerAngles& eulerAnglesTo, float alpha, short epsilon)
    {
        *this = Lerp(*this, eulerAnglesTo, alpha, epsilon);
    }

    EulerAngles EulerAngles::Lerp(const EulerAngles& from, const EulerAngles& to, float alpha, short epsilon)
    {
        return EulerAngles(Lerp(from.x, to.x, alpha, epsilon),
                           Lerp(from.y, to.y, alpha, epsilon),
                           Lerp(from.z, to.z, alpha, epsilon));
    }

    void EulerAngles::Slerp(const EulerAngles& to, float alpha)
    {
        *this = Slerp(*this, to, alpha);
    }

    EulerAngles EulerAngles::Slerp(const EulerAngles& from, const EulerAngles& to, float alpha)
    {
        auto quatFrom = from.ToQuaternion();
        auto quatTo = to.ToQuaternion();

        auto quat = glm::slerp(quatFrom, quatTo, alpha);
        return EulerAngles(quat);
    }

    void EulerAngles::InterpolateConstant(const EulerAngles& to, short angularVel)
    {
        *this = InterpolateConstant(*this, to, angularVel);
    }

    EulerAngles EulerAngles::InterpolateConstant(const EulerAngles& from, const EulerAngles& to, short angularVel)
    {
        return EulerAngles(InterpConstant(from.x, to.x, angularVel),
                           InterpConstant(from.y, to.y, angularVel),
                           InterpConstant(from.z, to.z, angularVel));
    }

    glm::vec3 EulerAngles::ToDirection() const
    {
        constexpr auto FORWARD_DIR = glm::vec3(0.0f, 0.0f, 1.0f);
    
        auto rotMat = glm::yawPitchRoll(FP_ANGLE_TO_RAD(y), FP_ANGLE_TO_RAD(x), FP_ANGLE_TO_RAD(z));
        return glm::normalize(glm::mat3(rotMat) * FORWARD_DIR);
    }

    AxisAngle EulerAngles::ToAxisAngle() const
    {
        return AxisAngle(*this);
    }

    glm::quat EulerAngles::ToQuaternion() const
    {
        return glm::quat(glm::vec3(FP_ANGLE_TO_RAD(x), FP_ANGLE_TO_RAD(y), FP_ANGLE_TO_RAD(z)));
    }

    glm::mat3 EulerAngles::ToRotationMatrix() const
    {
        return glm::mat3(glm::yawPitchRoll(FP_ANGLE_TO_RAD(y), FP_ANGLE_TO_RAD(x), FP_ANGLE_TO_RAD(z)));
    }

    bool EulerAngles::operator ==(const EulerAngles& eulerAngles) const
    {
        return x == eulerAngles.x && y == eulerAngles.y && z == eulerAngles.z;
    }

    bool EulerAngles::operator !=(const EulerAngles& eulerAngles) const
    {
        return !(*this == eulerAngles);
    }

    EulerAngles& EulerAngles::operator =(const EulerAngles& eulerAngles)
    {
        x = eulerAngles.x;
        y = eulerAngles.y;
        z = eulerAngles.z;
        return *this;
    }

    EulerAngles& EulerAngles::operator +=(const EulerAngles& eulerAngles)
    {
        x += eulerAngles.x;
        y += eulerAngles.y;
        z += eulerAngles.z;
        return *this;
    }

    EulerAngles& EulerAngles::operator -=(const EulerAngles& eulerAngles)
    {
        x -= eulerAngles.x;
        y -= eulerAngles.y;
        z -= eulerAngles.z;
        return *this;
    }

    EulerAngles& EulerAngles::operator *=(const EulerAngles& eulerAngles)
    {
        x *= eulerAngles.x;
        y *= eulerAngles.y;
        z *= eulerAngles.z;
        return *this;
    }

    EulerAngles& EulerAngles::operator *=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    EulerAngles& EulerAngles::operator /=(float scalar)
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    EulerAngles EulerAngles::operator +(const EulerAngles& eulerAngles) const
    {
        return EulerAngles(x + eulerAngles.x, y + eulerAngles.y, z + eulerAngles.z);
    }

    EulerAngles EulerAngles::operator -(const EulerAngles& eulerAngles) const
    {
        return EulerAngles(x - eulerAngles.x, y - eulerAngles.y, z - eulerAngles.z);
    }

    EulerAngles EulerAngles::operator *(const EulerAngles& eulerAngles) const
    {
        return EulerAngles(x * eulerAngles.x, y * eulerAngles.y, z * eulerAngles.z);
    }

    EulerAngles EulerAngles::operator *(float scalar) const
    {
        return EulerAngles((short)round(x * scalar), (short)round(y * scalar), (short)round(z * scalar));
    }

    EulerAngles EulerAngles::operator /(float scalar) const
    {
        return EulerAngles((short)round(x / scalar), (short)round(y / scalar), (short)round(z / scalar));
    }

    bool EulerAngles::Compare(short angle0, short angle1, short epsilon)
    {
        short delta = angle1 - angle0;
        return abs(delta) <= epsilon;
    }

    short EulerAngles::Lerp(short angleFrom, short angleTo, float alpha, short epsilon)
    {
        if (Compare(angleFrom, angleTo, epsilon))
        {
            return angleTo;
        }

        short angleDelta = angleTo - angleFrom;
        return (short)round(angleFrom + (angleDelta * std::clamp(alpha, 0.0f, 1.0f)));
    }

    short EulerAngles::InterpConstant(short angleFrom, short angleTo, short angularVel)
    {
        if (Compare(angleFrom, angleTo, angularVel))
        {
            return angleTo;
        }

        int sign = copysign(1, angleTo - angleFrom);
        return angleFrom + (angularVel * sign);
    }
}
