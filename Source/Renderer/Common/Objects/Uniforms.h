#pragma once

#include "Renderer/Common/Objects/Layouts/UniformBlit.h"
#include "Renderer/Common/Objects/Layouts/UniformCrt.h"
#include "Renderer/Common/Objects/Layouts/UniformGlyph2d.h"
#include "Renderer/Common/Objects/Layouts/UniformLumaFade.h"
#include "Renderer/Common/Objects/Layouts/UniformSprite2d.h"

namespace Silent::Renderer
{
    using UniformType = std::variant<UniformBlit,
                                     UniformCrt,
                                     UniformGlyph2d,
                                     UniformLumaFade,
                                     UniformSprite2d>;
}
