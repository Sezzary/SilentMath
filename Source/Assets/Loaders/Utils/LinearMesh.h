#pragma once

#include "Renderer/Common/Resources/Layouts/Buffers.h"

using namespace Silent::Renderer;
namespace Silent::Assets
{
    /** @brief GPU-compatible linear mesh. */
    struct LinearMesh
    {
        std::vector<BufferVertex3d> Vertices = {};
        std::vector<uint16>         Idxs     = {};
    };
}
