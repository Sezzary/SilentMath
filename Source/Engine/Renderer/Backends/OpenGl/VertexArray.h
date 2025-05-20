#pragma once

#include "Engine/Renderer/Backends/OpenGl/VertexBuffer.h"

namespace Silent::Renderer
{
    class VertexArrayManager
    {
    public:
        uint Id = 0;

        // Constructors

        VertexArrayManager() = default;

        // Utilities

        void Initialize();
        void LinkVertexBuffer(VertexBufferManager& vertBuffer, uint layout);
        void Bind();
        void Unbind();
        void Delete();
    };
}
