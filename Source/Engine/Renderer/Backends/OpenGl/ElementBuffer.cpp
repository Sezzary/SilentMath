#include "Framework.h"
#include "Engine/Renderer/Backends/OpenGl/ElementBuffer.h"

namespace Silent::Renderer
{
    void ElementBufferObject::Initialize(const std::span<uint>& idxs, GLenum usage)
    {
        glGenBuffers(1, &_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxs.size_bytes(), idxs.data(), usage);
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
