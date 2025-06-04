#pragma once

namespace Silent::Renderer
{
    class ElementArrayBufferObject
    {
    private:
        // Fields

        uint _id = 0;

    public:
        // Constructors

        ElementArrayBufferObject() = default;

        // Utilities

        void Initialize(const std::span<uint>& idxs);
        void Bind();
        void Unbind();
        void Delete();
    };
}
