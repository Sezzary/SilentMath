#pragma once

namespace Silent::Renderer
{
    /** @brief 3D world triangle. */
    struct Triangle3d
    {
        std::array<int, TRI_VERTEX_COUNT> VertexIdxs = {};
    };
}
