#pragma once

#include "Renderer/Common/Enums.h"

namespace Silent::Renderer
{
    constexpr int TRI_3D_COUNT_MAX = SHRT_MAX / 3;

    /** @brief 3D world triangle. */
    struct Triangle3d
    {
        struct Triangle3dVertex
        {
            Vector3 Position = Vector3::Zero;
            Vector3 Normal   = Vector3::One;
            Color   Col      = Color::Clear;
            Vector2 Uv       = Vector2::Zero;
        };

        std::array<Triangle3dVertex, TRI_VERTEX_COUNT> Vertices    = {};
        std::string                                    TextureName = {};
        RenderStage                                    RenderStg   = RenderStage::Model;
        BlendMode                                      BlendMd     = BlendMode::Opaque;
    };
}
