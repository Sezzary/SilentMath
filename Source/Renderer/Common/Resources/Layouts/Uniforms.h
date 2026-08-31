#pragma once

#include "Renderer/Common/Resources/Layouts/Uniforms/Blit.h"
#include "Renderer/Common/Resources/Layouts/Uniforms/Crt.h"
#include "Renderer/Common/Resources/Layouts/Uniforms/Dither.h"
#include "Renderer/Common/Resources/Layouts/Uniforms/FilmGrain.h"
#include "Renderer/Common/Resources/Layouts/Uniforms/Fxaa.h"
#include "Renderer/Common/Resources/Layouts/Uniforms/Glyph2d.h"
#include "Renderer/Common/Resources/Layouts/Uniforms/LumaFade.h"
#include "Renderer/Common/Resources/Layouts/Uniforms/Material.h"
#include "Renderer/Common/Resources/Layouts/Uniforms/Model.h"
#include "Renderer/Common/Resources/Layouts/Uniforms/Primitive2d.h"
#include "Renderer/Common/Resources/Layouts/Uniforms/Primitive3d.h"
#include "Renderer/Common/Resources/Layouts/Uniforms/Sprite2d.h"
#include "Renderer/Common/Resources/Layouts/Uniforms/Vignette.h"

namespace Silent::Renderer
{
    using UniformType = std::variant<UniformBlitPerFrame,
                                     UniformCrtPerFrame,
                                     UniformDitherPerFrame,
                                     UniformFilmGrainPerFrame,
                                     UniformFxaaPerFrame,
                                     UniformGlyph2dPerObject,
                                     UniformLumaFadePerFrame,
                                     UniformMaterial,
                                     UniformModelPerObject,
                                     UniformPrimitive2dPerFrame,
                                     UniformPrimitive2dPerObject,
                                     UniformPrimitive3dPerFrame,
                                     UniformPrimitive3dPerObject,
                                     UniformSprite2dPerObject,
                                     UniformVignettePerFrame>;

    /** @brief GPU uniform buffer slots. @todo Can't use this as uniform buffers must be packed flatly in the shader. */
    enum class UniformSlot
    {
        PerFrame    = 0,
        PerObject   = 1,
        PerMaterial = 2
    };
}
