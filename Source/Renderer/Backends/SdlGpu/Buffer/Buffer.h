#pragma once

namespace Silent::Renderer
{
    /** @brief GPU vertex, index, or indirect buffer. */
    template <typename T>
    class Buffer
    {
    private:
        // =======
        // Fields
        // =======

        SDL_GPUDevice*          _device         = nullptr;
        SDL_GPUBuffer*          _resourceBuffer = nullptr;
        SDL_GPUTransferBuffer*  _transferBuffer = nullptr;
        SDL_GPUBufferUsageFlags _usageFlags     = 0;

    public:
        // =============
        // Constructors
        // =============

        /** @brief Constructs an uninitialized default `Buffer`. */
        Buffer() = default;

        /** @brief Constructs a vertex, index, or indirect `Buffer`.
         *
         * @param device GPU device.
         * @param usageFlags Buffer usage flags.
         * @param size Max buffer size in number of elements.
         * @param name Buffer name.
         */
        Buffer(SDL_GPUDevice& device, SDL_GPUBufferUsageFlags usageFlags, uint size, const std::string& name = {});

        /** @brief Gracefully destroys the `Buffer` and releases GPU resources. */
        ~Buffer();

        // ==========
        // Utilities
        // ==========

        /** @brief Uploads data to the GPU buffer.
         *
         * @param data New data to transfer to the buffer.
         * @param startIdx Start index in the buffer at which to insert the new data.
         */
        void Update(SDL_GPUCopyPass& copyPass, const std::span<const T>& data, uint startIdx);

        /** @brief Binds the GPU buffer for drawing.
         *
         * @param renderPass Render pass.
         * @param startIdx Data start index.
         */
        void Bind(SDL_GPURenderPass& renderPass, uint startIdx);

    private:
        // ========
        // Helpers
        // ========

        /** @brief Checks if the GPU buffer is valid and ready to use.
         *
         * @return `true` if valid, `false` otherwise.
         */
        bool IsValid() const;
    };

    template <typename T>
    Buffer<T>::Buffer(SDL_GPUDevice& device, SDL_GPUBufferUsageFlags usageFlags, uint size, const std::string& name)
    {
        _usageFlags = usageFlags;
        if (!(_usageFlags & (SDL_GPU_BUFFERUSAGE_VERTEX | SDL_GPU_BUFFERUSAGE_INDEX | SDL_GPU_BUFFERUSAGE_INDIRECT)))
        {
            throw std::runtime_error("Attempted to create GPU buffer with invalid usage flags.");
        }

        _device = &device;

        auto bufferInfo = SDL_GPUBufferCreateInfo
        {
            .usage = _usageFlags,
            .size  = size * sizeof(T)
        };

        // Create buffer.
        _resourceBuffer = SDL_CreateGPUBuffer(&device, &bufferInfo);
        if (_resourceBuffer == nullptr)
        {
            Debug::Log(Fmt("Failed to create buffer `{}`: {}", name, SDL_GetError()), Debug::LogLevel::Error);
            // @todo Handle error?
        }
        SDL_SetGPUBufferName(_device, _resourceBuffer, name.c_str());

        auto transferBufferInfo = SDL_GPUTransferBufferCreateInfo
        {
            .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
            .size  = size * sizeof(T)
        };

        // Create transfer buffer.
        _transferBuffer = SDL_CreateGPUTransferBuffer(_device, &transferBufferInfo);
        if (_transferBuffer == nullptr)
        {
            Debug::Log(Fmt("Failed to create transfer buffer `{}`: {}", name, SDL_GetError()), Debug::LogLevel::Error);
        }
    }

    template <typename T>
    Buffer<T>::~Buffer()
    {
        // @todo Crashes??
        /*if (_resourceBuffer != nullptr)
        {
            SDL_ReleaseGPUBuffer(_device, _resourceBuffer);
        }

        if (_transferBuffer != nullptr)
        {
            SDL_ReleaseGPUTransferBuffer(_device, _transferBuffer);
        }*/
    }

    template <typename T>
    void Buffer<T>::Update(SDL_GPUCopyPass& copyPass, const std::span<const T>& data, uint startIdx)
    {
        if (!IsValid())
        {
            Debug::Log("Attempted to update invalid GPU buffer.", Debug::LogLevel::Warning);
            return;
        }

        // Map transfer data.
        auto* mappedTransferData = (T*)SDL_MapGPUTransferBuffer(_device, _transferBuffer, false);
        memcpy(mappedTransferData, data.data(), data.size_bytes());
        SDL_UnmapGPUTransferBuffer(_device, _transferBuffer);

        // Upload data.
        auto transferBufferLoc = SDL_GPUTransferBufferLocation
        {
            .transfer_buffer = _transferBuffer
        };
        auto bufferRegion = SDL_GPUBufferRegion
        {
            .buffer = _resourceBuffer,
            .offset = startIdx * sizeof(T),
            .size   = (uint)data.size_bytes()
        };
        SDL_UploadToGPUBuffer(&copyPass, &transferBufferLoc, &bufferRegion, true);
    }

    template <typename T>
    void Buffer<T>::Bind(SDL_GPURenderPass& renderPass, uint startIdx)
    {
        if (!IsValid())
        {
            Debug::Log("Attempted to bind invalid GPU buffer.", Debug::LogLevel::Warning);
            return;
        }

        auto bufferBindings = SDL_GPUBufferBinding
        {
            .buffer = _resourceBuffer,
            .offset = startIdx * sizeof(T)
        };

        if (_usageFlags & SDL_GPU_BUFFERUSAGE_VERTEX)
        {
            SDL_BindGPUVertexBuffers(&renderPass, 0, &bufferBindings, 1);
        }
        else if (_usageFlags & SDL_GPU_BUFFERUSAGE_INDEX)
        {
            SDL_BindGPUIndexBuffer(&renderPass, &bufferBindings, SDL_GPU_INDEXELEMENTSIZE_16BIT);
        }
        else if (_usageFlags & SDL_GPU_BUFFERUSAGE_INDIRECT)
        {
            Debug::Log("Attempted to bind indirect GPU buffer.", Debug::LogLevel::Info);
        }
    }

    template <typename T>
    bool Buffer<T>::IsValid() const
    {
        return _resourceBuffer != nullptr && _transferBuffer != nullptr;
    }
};
