#pragma once

namespace Silent::Renderer
{
    /** @brief GPU buffer for 3D triangle. */
    struct BufferTriangle3d
    {
        std::array<int, TRI_VERTEX_COUNT> Idxs = {};
    };
}
