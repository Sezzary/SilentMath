#include "Framework.h"
#include "Engine/Renderer/Backends/OpenGl/VertexArray.h"

#include "Engine/Renderer/Backends/OpenGl/VertexBuffer.h"

namespace Silent::Renderer
{
    void VertexArrayManager::Initialize()
    {
        glGenVertexArrays(1, &Id);
    }

    void VertexArrayManager::LinkVertexBuffer(VertexBufferManager& vertBuffer, uint layout)
    {
        vertBuffer.Bind();
        glVertexAttribPointer(layout, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(layout);
        vertBuffer.Unbind();
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
}
