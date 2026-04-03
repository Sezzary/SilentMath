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
        BlendMode                                      BlendMd     = BlendMode::Opaque;

        static Triangle3d CreateTriangle3d(const Vector3& vert0, const Vector3& vert1, const Vector3& vert2,
                                           const Color& color,
                                           BlendMode blendMode = BlendMode::Opaque);
    };
}
