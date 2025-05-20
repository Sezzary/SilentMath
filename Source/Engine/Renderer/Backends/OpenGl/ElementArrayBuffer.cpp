#include "Framework.h"
#include "Engine/Renderer/Backends/OpenGl/ElementArrayBuffer.h"

namespace Silent::Renderer
{
    void ElementArrayBufferManager::Initialize(uint* idxs, uint size)
    {
        glGenBuffers(1, &Id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, idxs, GL_STATIC_DRAW);
    }

    void ElementArrayBufferManager::Bind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Id);
    }

    void ElementArrayBufferManager::Unbind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void ElementArrayBufferManager::Delete()
    {
        glDeleteBuffers(1, &Id);
    }
}