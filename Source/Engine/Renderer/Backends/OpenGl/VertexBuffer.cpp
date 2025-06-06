#include "Framework.h"
#include "Engine/Renderer/Backends/OpenGl/VertexBuffer.h"

namespace Silent::Renderer
{
    void VertexBufferObject::Initialize(const std::span<float>& verts, GLenum usage)
    {
        glGenBuffers(1, &_id);
        glBindBuffer(GL_ARRAY_BUFFER, _id);
        glBufferData(GL_ARRAY_BUFFER, verts.size_bytes(), verts.data(), usage);
    }

    void VertexBufferObject::Bind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, _id);
    }

    void VertexBufferObject::Unbind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void VertexBufferObject::Delete()
    {
        glDeleteBuffers(1, &_id);
    }
}