#pragma once

#include "Renderer/Backends/SdlGpu/Resources/Buffer.h"

namespace Silent::Renderer::SdlGpu
{
    /** @brief Indexed GPU vertex buffer with 16-bit indices. */
    template <typename T>
    class VertexBuffer
    {
    private:
        // =======
        // Fields
        // =======

        Buffer<T>      _vertexBuffer = {};
        Buffer<uint16> _idxBuffer    = {};

    public:
        // =============
        // Constructors
        // =============

        VertexBuffer() = default;

        // ========
        // Getters
        // ========

        /** @brief Gets the name of the GPU vertex buffer.
         *
         * @return GPU buffer name.
         */
        const std::string& GetName() const;

        /** @brief Gets the vertex capacity in the GPU vertex buffer.
         *
         * @return Vertex capacity.
         */
        int GetVertexCapacity() const;

        /** @brief Gets the vertex index capacity in the GPU vertex buffer.
         *
         * @return Vertex index capacity.
         */
        int GetIdxCapacity() const;

        // ==========
        // Utilities
        // ==========

        /** @brief Initializes the indexed GPU vertex buffer.
         *
         * @param device GPU device.
         * @param vertCount Vertex count.
         * @param idxCount Index count.
         * @param name Buffer name.
         */
        void Initialize(SDL_GPUDevice& device, int vertCount, int idxCount, const std::string& name = {});

        /** @brief Uploads vertices to the internal GPU vertex buffer.
         *
         * @param verts New vertices to transfer to the vertex buffer.
         * @param offset Start index in the vertex buffer at which to insert the new vertices.
         */
        void UpdateVertices(SDL_GPUCopyPass& copyPass, std::span<const T> verts, int offset);

        /** @brief Uploads indices to the internal GPU index buffer.
         *
         * @param idxs New indices to transfer to the index buffer.
         * @param offset Start index in the index buffer at which to insert the new indices.
         */
        void UpdateIdxs(SDL_GPUCopyPass& copyPass, std::span<const uint16> idxs, int offset);

        /** @brief Binds the indexed GPU vertex buffer for drawing.
         *
         * @param renderPass Render pass.
         * @param vertOffset Vertices start index.
         * @param idxOffset Indices start index.
         */
        void Bind(SDL_GPURenderPass& renderPass, int vertOffset, int idxOffset);
    };

    template <typename T>
    const std::string& VertexBuffer<T>::GetName() const
    {
        return _vertexBuffer.GetName();
    }

    template <typename T>
    int VertexBuffer<T>::GetVertexCapacity() const
    {
        return _vertexBuffer.GetCapacity();
    }

    template <typename T>
    int VertexBuffer<T>::GetIdxCapacity() const
    {
        return _idxBuffer.GetCapacity();
    }

    template <typename T>
    void VertexBuffer<T>::Initialize(SDL_GPUDevice& device, int vertCount, int idxCount, const std::string& name)
    {
        _vertexBuffer.Initialize(device, SDL_GPU_BUFFERUSAGE_VERTEX, vertCount, name);
        _idxBuffer.Initialize(device, SDL_GPU_BUFFERUSAGE_INDEX, idxCount, name + " (indices)");
    }

    template <typename T>
    void VertexBuffer<T>::UpdateVertices(SDL_GPUCopyPass& copyPass, std::span<const T> verts, int offset)
    {
        _vertexBuffer.Update(copyPass, verts, offset);
    }

    template <typename T>
    void VertexBuffer<T>::UpdateIdxs(SDL_GPUCopyPass& copyPass, std::span<const uint16> idxs, int offset)
    {
        _idxBuffer.Update(copyPass, idxs, offset);
    }

    template <typename T>
    void VertexBuffer<T>::Bind(SDL_GPURenderPass& renderPass, int vertOffset, int idxOffset)
    {
        _vertexBuffer.Bind(renderPass, vertOffset);
        _idxBuffer.Bind(renderPass, idxOffset);
    }
}
