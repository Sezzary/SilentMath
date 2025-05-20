#pragma once

namespace Silent::Renderer
{
    class VertexBufferManager
    {
    public:
        uint Id = 0;

        // Constructors

        VertexBufferManager() = default;

        // Utilities

        void Initialize(float* verts, uint size);
        void Bind();
        void Unbind();
        void Delete();
    };
}
