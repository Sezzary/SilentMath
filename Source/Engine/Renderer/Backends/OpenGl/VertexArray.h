#pragma once

#include "Engine/Renderer/Backends/OpenGl/VertexBuffer.h"

namespace Silent::Renderer
{
    class VertexArrayManager
    {
    public:
        // Fields

        uint Id = 0;

        // Constructors

        VertexArrayManager() = default;

        // Utilities

        void Initialize();
        void Bind();
        void Unbind();
        void Delete();

        void LinkAttrib(VertexBufferManager& vertBuffer, uint layout, uint componentCount, GLenum type, GLsizeiptr stride, void* offset);
    };
}
