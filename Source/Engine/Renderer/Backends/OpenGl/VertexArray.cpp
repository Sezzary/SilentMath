#include "Framework.h"
#include "Engine/Renderer/Backends/OpenGl/VertexArray.h"

#include "Engine/Renderer/Backends/OpenGl/VertexBuffer.h"

namespace Silent::Renderer
{
    void VertexArrayManager::Initialize()
    {
        glGenVertexArrays(1, &Id);
    }

    void VertexArrayManager::Bind()
    {
        glBindVertexArray(Id);
    }

    void VertexArrayManager::Unbind()
    {
        glBindVertexArray(0);
    }

    void VertexArrayManager::Delete()
    {
        glDeleteVertexArrays(1, &Id);
    }
    
    void VertexArrayManager::LinkAttrib(VertexBufferManager& vertBuffer, uint layout, uint componentCount, GLenum type, GLsizeiptr stride, void* offset)
    {
        vertBuffer.Bind();
        glVertexAttribPointer(layout, componentCount, type, GL_FALSE, stride, offset);
        glEnableVertexAttribArray(layout);
        vertBuffer.Unbind();
    }
}
