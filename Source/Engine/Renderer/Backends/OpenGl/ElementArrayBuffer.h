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

        void Initialize(const std::span<uint>& idxs);
        void Bind();
        void Unbind();
        void Delete();
    };
}
