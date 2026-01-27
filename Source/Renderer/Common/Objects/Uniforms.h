#pragma once

#include "Renderer/Common/Objects/Layouts/UniformCrt.h"
#include "Renderer/Common/Objects/Layouts/UniformFade.h"
#include "Renderer/Common/Objects/Layouts/UniformGlyph2d.h"
#include "Renderer/Common/Objects/Layouts/UniformSprite2d.h"

namespace Silent::Renderer
{
    using UniformType = std::variant<UniformCrt,
                                     UniformFade,
                                     UniformGlyph2d,
                                     UniformSprite2d>;
}
