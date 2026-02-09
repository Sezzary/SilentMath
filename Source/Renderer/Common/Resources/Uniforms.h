#pragma once

#include "Renderer/Common/Resources/Layouts/UniformBlit.h"
#include "Renderer/Common/Resources/Layouts/UniformCrt.h"
#include "Renderer/Common/Resources/Layouts/UniformGlyph2d.h"
#include "Renderer/Common/Resources/Layouts/UniformLumaFade.h"
#include "Renderer/Common/Resources/Layouts/UniformMaterial.h"
#include "Renderer/Common/Resources/Layouts/UniformModel.h"
#include "Renderer/Common/Resources/Layouts/UniformPrimitive3d.h"
#include "Renderer/Common/Resources/Layouts/UniformSprite2d.h"
#include "Renderer/Common/Resources/Layouts/UniformView.h"

namespace Silent::Renderer
{
    using UniformType = std::variant<UniformBlit,
                                     UniformCrt,
                                     UniformGlyph2d,
                                     UniformLumaFade,
                                     UniformMaterial,
                                     UniformModel,
                                     UniformPrimitive3d,
                                     UniformSprite2d,
                                     UniformView>;

    /** @brief GPU uniform buffer slots. */
    enum class UniformSlot
    {
        PerFrame    = 0,
        PerObject   = 1,
        PerMaterial = 2
    };
}
