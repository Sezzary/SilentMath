#pragma once

#include "Renderer/Backends/SdlGpu/Buffer/Buffer.h"

namespace Silent::Renderer
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
        void Initialize(SDL_GPUDevice& device, uint vertCount, uint idxCount, const std::string& name = {});

        /** @brief Uploads vertices to the internal GPU vertex buffer.
         *
         * @param verts New vertices to transfer to the vertex buffer.
         * @param startIdx Start index in the vertex buffer at which to insert the new vertices.
         */
        void UpdateVertices(SDL_GPUCopyPass& copyPass, const std::span<const T>& verts, uint startIdx);

        /** @brief Uploads indices to the internal GPU index buffer.
         *
         * @param idxs New indices to transfer to the index buffer.
         * @param startIdx Start index in the index buffer at which to insert the new indices.
         */
        void UpdateIdxs(SDL_GPUCopyPass& copyPass, const std::span<const uint16>& idxs, uint startIdx);

        /** @brief Binds the indexed GPU vertex buffer for drawing.
         *
         * @param renderPass Render pass.
         * @param vertsStartIdx Vertices start index.
         * @param idxsStartIdx Indices start index.
         */
        void Bind(SDL_GPURenderPass& renderPass, uint vertsStartIdx, uint idxsStartIdx);
    };

    template <typename T>
    void VertexBuffer<T>::Initialize(SDL_GPUDevice& device, uint vertCount, uint idxCount, const std::string& name)
    {
        _vertexBuffer.Initialize(device, SDL_GPU_BUFFERUSAGE_VERTEX, vertCount, name);
        _idxBuffer.Initialize(device, SDL_GPU_BUFFERUSAGE_INDEX, idxCount, name + " (indices)");
    }

    template <typename T>
    void VertexBuffer<T>::UpdateVertices(SDL_GPUCopyPass& copyPass, const std::span<const T>& verts, uint startIdx)
    {
        _vertexBuffer.Update(copyPass, verts, startIdx);
    }

    template <typename T>
    void VertexBuffer<T>::UpdateIdxs(SDL_GPUCopyPass& copyPass, const std::span<const uint16>& idxs, uint startIdx)
    {
        _idxBuffer.Update(copyPass, idxs, startIdx);
    }

    template <typename T>
    void VertexBuffer<T>::Bind(SDL_GPURenderPass& renderPass, uint vertsStartIdx, uint idxsStartIdx)
    {
        _vertexBuffer.Bind(renderPass, vertsStartIdx);
        _idxBuffer.Bind(renderPass, idxsStartIdx);
    }
}
