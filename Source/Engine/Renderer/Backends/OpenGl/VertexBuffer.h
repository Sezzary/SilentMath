#pragma once

namespace Silent::Renderer
{
    class VertexBufferObject
    {
    private:
        // Fields

        uint _id = 0;

    public:
        // Constructors

        VertexBufferObject() = default;

        // Utilities

        void Initialize(const std::span<float>& verts, GLenum usage = GL_STATIC_DRAW);
        void Bind();
        void Unbind();
        void Delete();
    };
}
