#include "Framework.h"
#include "Engine/Renderer/Backends/OpenGl/VertexBuffer.h"

namespace Silent::Renderer
{
    void VertexBufferManager::Initialize(const std::span<float>& verts)
    {
        glGenBuffers(1, &Id);
        glBindBuffer(GL_ARRAY_BUFFER, Id);
        glBufferData(GL_ARRAY_BUFFER, verts.size(), verts.data(), GL_STATIC_DRAW);
    }

    void VertexBufferManager::Bind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, Id);
    }

    void VertexBufferManager::Unbind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void VertexBufferManager::Delete()
    {
        glDeleteBuffers(1, &Id);
    }
}