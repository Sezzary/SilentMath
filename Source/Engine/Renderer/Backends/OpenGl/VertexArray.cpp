#include "Framework.h"
#include "Engine/Renderer/Backends/OpenGl/VertexArray.h"

#include "Engine/Renderer/Backends/OpenGl/VertexBuffer.h"

namespace Silent::Renderer
{
    void VertexArrayObject::Initialize()
    {
        glGenVertexArrays(1, &_id);
    }

    void VertexArrayObject::Bind()
    {
        glBindVertexArray(_id);
    }

    void VertexArrayObject::Unbind()
    {
        glBindVertexArray(0);
    }

    void VertexArrayObject::Delete()
    {
        glDeleteVertexArrays(1, &_id);
    }
    
    void VertexArrayObject::LinkAttrib(VertexBufferObject& vbo, uint layoutId, uint componentCount, GLenum type, GLsizeiptr stride, void* offset)
    {
        vbo.Bind();
        glVertexAttribPointer(layoutId, componentCount, type, GL_FALSE, stride, offset);
        glEnableVertexAttribArray(layoutId);
        vbo.Unbind();
    }
}
