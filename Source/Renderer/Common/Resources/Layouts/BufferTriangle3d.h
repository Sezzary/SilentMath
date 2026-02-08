#pragma once

namespace Silent::Renderer
{
    /** @brief GPU buffer for 3D triangle. */
    struct BufferTriangle3d
    {
        std::array<Vector3, 3> Idxs = {};
    };
}
