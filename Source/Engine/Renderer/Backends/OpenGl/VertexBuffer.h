#pragma once

namespace Silent::Renderer
{
    class VertexBufferManager
    {
    public:
        // Fields

        GLuint Id = 0;

        // Constructors

        VertexBufferManager() = default;

        // Utilities

        void Initialize(GLfloat* verts, GLsizeiptr size);
        void Bind();
        void Unbind();
        void Delete();
    };
}
