#pragma once

#include "Renderer/Backends/OpenGl/Texture.h"
#include "Renderer/Backends/OpenGl/VertexArray.h"
#include "Renderer/Backends/OpenGl/VertexBuffer.h"

namespace Silent::Renderer
{
    class FramebufferObject
    {
    private:
        // Fields

        uint               _frameBufferId = 0;
        TextureGl            _texture       = TextureGl();
        VertexArrayObject  _vao           = VertexArrayObject();
        VertexBufferObject _vbo           = VertexBufferObject();

    public:
        // Constructors

        FramebufferObject() = default;

        // Utilities

        void Initialize();
        void Update();
        void Bind();
        void Unbind();
        void Delete();
    };
}
