#pragma once

namespace Silent::Renderer
{
    class ElementArrayBufferManager
    {
    public:
        uint Id = 0;

        // Constructors

        ElementArrayBufferManager() = default;

        // Utilities

        void Initialize(uint* idxs, uint size);
        void Bind();
        void Unbind();
        void Delete();
    };
}
