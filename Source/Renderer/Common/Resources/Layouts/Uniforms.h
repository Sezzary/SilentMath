#pragma once

#include "Renderer/Common/Resources/Layouts/Uniforms/Blit.h"
#include "Renderer/Common/Resources/Layouts/Uniforms/Crt.h"
#include "Renderer/Common/Resources/Layouts/Uniforms/Glyph2d.h"
#include "Renderer/Common/Resources/Layouts/Uniforms/LumaFade.h"
#include "Renderer/Common/Resources/Layouts/Uniforms/Material.h"
#include "Renderer/Common/Resources/Layouts/Uniforms/Model.h"
#include "Renderer/Common/Resources/Layouts/Uniforms/Primitive3d.h"
#include "Renderer/Common/Resources/Layouts/Uniforms/Sprite2d.h"
#include "Renderer/Common/Resources/Layouts/Uniforms/View.h"

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

    // @todo Uniform buffers need to be packed flatly in the shader.
    /** @brief GPU uniform buffer slots. */
    enum class UniformSlot
    {
        PerFrame    = 0,
        PerObject   = 1,
        PerMaterial = 2
    };
}
