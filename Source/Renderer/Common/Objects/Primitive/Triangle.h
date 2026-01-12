#pragma once

namespace Silent::Renderer
{
    /** @brief 2D or 3D triangle. */
    struct Triangle
    {
        constexpr int VERTEX_COUNT = 3;

        std::array<int, VERTEX_COUNT> VertexIdxs = {};
    };
}
