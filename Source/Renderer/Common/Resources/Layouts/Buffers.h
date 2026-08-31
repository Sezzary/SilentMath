#pragma once

#include "Renderer/Common/Resources/Layouts/Buffers/Triangle3d.h"
#include "Renderer/Common/Resources/Layouts/Buffers/Vertex2d.h"
#include "Renderer/Common/Resources/Layouts/Buffers/Vertex3d.h"

namespace Silent::Renderer
{
    using BufferType = std::variant<BufferTriangle3d,
                                    BufferVertex2d,
                                    BufferVertex3d>;
}
