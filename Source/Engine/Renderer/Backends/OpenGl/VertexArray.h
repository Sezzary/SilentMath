#pragma once

#include "Engine/Renderer/Backends/OpenGl/VertexBuffer.h"

namespace Silent::Renderer
{
    class VertexArrayObject
    {
    private:
        // Fields

        uint _id = 0;

    public:
        // Constructors

        VertexArrayObject() = default;

        // Utilities

        void Initialize();
        void Bind();
        void Unbind();
        void Delete();

        void LinkAttrib(VertexBufferObject& vbo, uint layoutId, uint componentCount, GLenum type, uint stride, void* offset);
    };
}
