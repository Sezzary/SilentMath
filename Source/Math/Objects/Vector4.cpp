#include "Framework.h"
#include "Math/Objects/Vector4.h"

namespace Silent::Math
{
    const Vector4 Vector4::Zero  = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
    const Vector4 Vector4::One   = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    const Vector4 Vector4::UnitX = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
    const Vector4 Vector4::UnitY = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
    const Vector4 Vector4::UnitZ = Vector4(0.0f, 0.0f, 1.0f, 0.0f);
    const Vector4 Vector4::UnitW = Vector4(0.0f, 0.0f, 0.0f, 1.0f);

    const glm::vec4& Vector4::ToGlmVec4() const
    {
        return *(const glm::vec4*)this;
    }

    glm::vec4& Vector4::ToGlmVec4()
    {
        return *(glm::vec4*)this;
    }
}
