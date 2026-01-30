#pragma once

#include "Renderer/Common/Resources/Layouts/UniformBlit.h"
#include "Renderer/Common/Resources/Layouts/UniformCrt.h"
#include "Renderer/Common/Resources/Layouts/UniformGlyph2d.h"
#include "Renderer/Common/Resources/Layouts/UniformLumaFade.h"
#include "Renderer/Common/Resources/Layouts/UniformSprite2d.h"

namespace Silent::Renderer
{
    using UniformType = std::variant<UniformBlit,
                                     UniformCrt,
                                     UniformGlyph2d,
                                     UniformLumaFade,
                                     UniformSprite2d>;
}
