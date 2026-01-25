#pragma once

#include "Renderer/Common/Objects/Layouts/UniformFade.h"
#include "Renderer/Common/Objects/Layouts/UniformGlyph2d.h"
#include "Renderer/Common/Objects/Layouts/UniformSprite2d.h"

namespace Silent::Renderer
{
    using UniformType = std::variant<UniformSprite2d,
                                     UniformGlyph2d,
                                     UniformFade>;
}
