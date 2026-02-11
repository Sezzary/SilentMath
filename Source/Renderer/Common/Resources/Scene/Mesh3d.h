#pragma once

#include "Renderer/Common/Resources/Primitive/Triangle3d.h"
#include "Renderer/Common/Resources/Primitive/Vertex3d.h"

namespace Silent::Renderer
{
    /** @brief 3D world mesh. @todo Should refactor this to use buckets later. */
    struct Mesh3d
    {
        std::vector<Triangle3d> Triangles = {};
        std::vector<Vertex3d>   Vertices  = {};
    };
}
