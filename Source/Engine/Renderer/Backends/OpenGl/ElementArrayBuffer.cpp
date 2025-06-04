#include "Framework.h"
#include "Engine/Renderer/Backends/OpenGl/ElementArrayBuffer.h"

namespace Silent::Renderer
{
    void ElementBufferObject::Initialize(const std::span<uint>& idxs)
    {
        glGenBuffers(1, &_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxs.size(), idxs.data(), GL_STATIC_DRAW);
    }

    void ElementBufferObject::Bind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _id);
    }

    void ElementBufferObject::Unbind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void ElementBufferObject::Delete()
    {
        glDeleteBuffers(1, &_id);
    }
}
