#pragma once

namespace Silent::Renderer
{
    class ElementBufferObject
    {
    private:
        // Fields

        uint _id = 0;

    public:
        // Constructors

        ElementBufferObject() = default;

        // Utilities

        void Initialize(const std::span<uint>& idxs, GLenum usage = GL_STATIC_DRAW);
        void Bind();
        void Unbind();
        void Delete();
    };
}
