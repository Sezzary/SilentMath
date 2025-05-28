#pragma once

namespace Silent::Renderer
{
    class ElementArrayBufferManager
    {
    public:
        GLuint Id = 0;

        // Constructors

        ElementArrayBufferManager() = default;

        // Utilities

        void Initialize(GLuint* idxs, GLsizeiptr size);
        void Bind();
        void Unbind();
        void Delete();
    };
}
