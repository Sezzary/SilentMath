#pragma once

namespace Silent::Renderer
{
    class VertexBufferManager
    {
    public:
        // Fields

        uint Id = 0;

        // Constructors

        VertexBufferManager() = default;

        // Utilities

        void Initialize(const std::span<float>& verts);
        void Bind();
        void Unbind();
        void Delete();
    };
}
