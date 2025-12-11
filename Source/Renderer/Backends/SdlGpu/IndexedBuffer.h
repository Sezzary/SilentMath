#pragma once

#include "Renderer/Backends/SdlGpu/Buffer.h"

namespace Silent::Renderer
{
    /** @brief Indexed GPU buffer. */
    template <typename T>
    class IndexedBuffer
    {
    private:
        // =======
        // Fields
        // =======

        Buffer<T>      _dataBuffer = {};
        Buffer<uint16> _idxBuffer  = {};

    public:
        // =============
        // Constructors
        // =============

        IndexedBuffer() = default;

        // ==========
        // Utilities
        // ==========

        /** @brief Constructs an `IndexedBuffer`.
         *
         * @param device GPU device.
         * @param usageFlags Buffer usage flags.
         * @param dataSize Max data size in number of elements.
         * @param idxCount Index count.
         * @param name Buffer name.
         */
        IndexedBuffer(SDL_GPUDevice& device, SDL_GPUBufferUsageFlags usageFlags, uint dataSize, uint idxCount, const std::string& name = {});

        /** @brief Uploads data to the indexed data buffer.
         *
         * @param data New data to transfer to the data buffer.
         * @param startIdx Start index in the data buffer at which to transfer the new data.
         */
        void UpdateData(SDL_GPUCopyPass& copyPass, const std::span<const T>& data, uint startIdx);

        /** @brief Uploads indices to the index buffer.
         *
         * @param idxs New indices to transfer to the index buffer.
         * @param startIdx Start index in the index buffer at which to transfer the new indices.
         */
        void UpdateIdxs(SDL_GPUCopyPass& copyPass, const std::span<const uint16>& idxs, uint startIdx);

        /** @brief Binds the indexed GPU buffer for drawing.
         *
         * @param renderPass Render pass.
         * @param dataStartIdx Data start index.
         * @param idxsStartIdx Indices start index.
         */
        void Bind(SDL_GPURenderPass& renderPass, uint dataStartIdx, uint idxsStartIdx);
    };

    template <typename T>
    IndexedBuffer<T>::IndexedBuffer(SDL_GPUDevice& device, SDL_GPUBufferUsageFlags usageFlags, uint dataSize, uint idxCount, const std::string& name)
    {
        // @todo Or hardcode it to be a vertex buffer?
        _dataBuffer = Buffer<T>(device, usageFlags, dataSize, name);
        _idxBuffer  = Buffer<uint16>(device, SDL_GPU_BUFFERUSAGE_INDEX, idxCount, name + " Indices");
    }

    template <typename T>
    void IndexedBuffer<T>::UpdateData(SDL_GPUCopyPass& copyPass, const std::span<const T>& data, uint startIdx)
    {
        _dataBuffer.Update(copyPass, data, startIdx);
    }

    template <typename T>
    void IndexedBuffer<T>::UpdateIdxs(SDL_GPUCopyPass& copyPass, const std::span<const uint16>& idxs, uint startIdx)
    {
        _idxBuffer.Update(copyPass, idxs, startIdx);
    }

    template <typename T>
    void IndexedBuffer<T>::Bind(SDL_GPURenderPass& renderPass, uint dataStartIdx, uint idxsStartIdx)
    {
        _dataBuffer.Bind(renderPass, dataStartIdx);
        _idxBuffer.Bind(renderPass, idxsStartIdx);
    }
}
