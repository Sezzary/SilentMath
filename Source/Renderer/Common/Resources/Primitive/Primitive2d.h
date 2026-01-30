#pragma once

#include "Renderer/Common/Enums.h"
#include "Renderer/Common/Resources/Primitive/Vertex2d.h"
#include "Renderer/Common/Resources/Uniforms.h"

namespace Silent::Renderer
{
    /** @brief 2D screen primitive representing a quad or triangle. */
    struct Primitive2d
    {
        std::vector<Vertex2d> Vertices    = {};
        int                   Depth       = 0;
        std::string           TextureName = {};
        RenderStage           RenderStg   = RenderStage::Sprite2d;
        BlendMode             BlendMd     = BlendMode::Opaque;
        UniformType           Uniform     = {};
    };
}
