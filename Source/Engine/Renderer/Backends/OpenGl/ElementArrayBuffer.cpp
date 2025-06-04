#include "Framework.h"
#include "Engine/Renderer/Backends/OpenGl/ElementArrayBuffer.h"

namespace Silent::Renderer
{
    void ElementArrayBufferManager::Initialize(const std::span<uint>& idxs)
    {
        glGenBuffers(1, &Id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxs.size(), idxs.data(), GL_STATIC_DRAW);
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
