#pragma once

#include "Renderer/Common/Resources/Layouts/Uniforms/Blit.h"
#include "Renderer/Common/Resources/Layouts/Uniforms/Crt.h"
#include "Renderer/Common/Resources/Layouts/Uniforms/Dither.h"
#include "Renderer/Common/Resources/Layouts/Uniforms/FilmGrain.h"
#include "Renderer/Common/Resources/Layouts/Uniforms/Glyph2d.h"
#include "Renderer/Common/Resources/Layouts/Uniforms/LumaFade.h"
#include "Renderer/Common/Resources/Layouts/Uniforms/Material.h"
#include "Renderer/Common/Resources/Layouts/Uniforms/Model.h"
#include "Renderer/Common/Resources/Layouts/Uniforms/Pixelize.h"
#include "Renderer/Common/Resources/Layouts/Uniforms/Primitive3d.h"
#include "Renderer/Common/Resources/Layouts/Uniforms/Sprite2d.h"
#include "Renderer/Common/Resources/Layouts/Uniforms/View.h"

namespace Silent::Renderer
{
    using UniformType = std::variant<UniformBlit,
                                     UniformCrt,
                                     UniformDither,
                                     UniformFilmGrain,
                                     UniformGlyph2d,
                                     UniformLumaFade,
                                     UniformMaterial,
                                     UniformModel,
                                     UniformPixelize,
                                     UniformPrimitive3d,
                                     UniformSprite2d,
                                     UniformView>;

    /** @brief GPU uniform buffer slots. @todo Can't use this as uniform buffers must be packed flatly in the shader. */
    enum class UniformSlot
    {
        PerFrame    = 0,
        PerObject   = 1,
        PerMaterial = 2
    };
}
