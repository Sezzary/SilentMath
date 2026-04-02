#pragma once

namespace Silent::Renderer::SdlGpu
{
    /** @brief GPU vertex, index, or indirect buffer. */
    template <typename T>
    class Buffer
    {
    private:
        // =======
        // Fields
        // =======

        std::string             _name           = {};
        int                     _capacity       = 0;
        SDL_GPUDevice*          _device         = nullptr;
        SDL_GPUBuffer*          _resourceBuffer = nullptr;
        SDL_GPUTransferBuffer*  _transferBuffer = nullptr;
        SDL_GPUBufferUsageFlags _usageFlags     = 0;

    public:
        // =============
        // Constructors
        // =============

        /** @brief Creates a default uninitialized instance. */
        Buffer() = default;

        /** @brief Gracefully destroys the instance and releases GPU resources. */
        ~Buffer();

        // ========
        // Getters
        // ========

        /** @brief Gets the name of the GPU buffer.
         *
         * @return GPU buffer name.
         */
        const std::string& GetName() const;

        /** @brief Gets the capacity the GPU buffer.
         *
         * @return Capacity in number of elements.
         */
        int GetCapacity() const;

        // ==========
        // Utilities
        // ==========

        /** @brief Initializes the GPU buffer.
         *
         * @param device GPU device.
         * @param usageFlags Buffer usage flags.
         * @param capacity Max number of elements.
         * @param name Buffer name.
         */
        void Initialize(SDL_GPUDevice& device, SDL_GPUBufferUsageFlags usageFlags, int capacity, const std::string& name = {});

        /** @brief Uploads data to the GPU buffer.
         *
         * @param data New data to transfer to the buffer.
         * @param offset Start index in the buffer at which to insert the new data.
         */
        void Update(SDL_GPUCopyPass& copyPass, std::span<const T> data, int offset);

        /** @brief Binds the GPU buffer for drawing.
         *
         * @param renderPass Render pass.
         * @param offset Data start index.
         */
        void Bind(SDL_GPURenderPass& renderPass, int offset);

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
    const std::string& Buffer<T>::GetName() const
    {
        return _name;
    }

    template <typename T>
    int Buffer<T>::GetCapacity() const
    {
        return _capacity;
    }

    template <typename T>
    Buffer<T>::~Buffer()
    {
        if (_resourceBuffer != nullptr)
        {
            SDL_ReleaseGPUBuffer(_device, _resourceBuffer);
        }

        if (_transferBuffer != nullptr)
        {
            SDL_ReleaseGPUTransferBuffer(_device, _transferBuffer);
        }
    }

    template <typename T>
    void Buffer<T>::Initialize(SDL_GPUDevice& device, SDL_GPUBufferUsageFlags usageFlags, int capacity, const std::string& name)
    {
        if (IsValid())
        {
            Debug::Log(Fmt("Attempted to reinitialize GPU buffer `{}` as `{}`.", _name, name), Debug::LogLevel::Warning);
            return;
        }

        _usageFlags = usageFlags;
        if (!(_usageFlags & (SDL_GPU_BUFFERUSAGE_VERTEX | SDL_GPU_BUFFERUSAGE_INDEX | SDL_GPU_BUFFERUSAGE_INDIRECT)))
        {
            throw std::runtime_error("Attempted to create GPU buffer with invalid usage flags.");
        }

        _name     = name;
        _capacity = capacity;
        _device   = &device;

        auto bufferInfo = SDL_GPUBufferCreateInfo
        {
            .usage = _usageFlags,
            .size  = capacity * sizeof(T)
        };

        // Create buffer.
        _resourceBuffer = SDL_CreateGPUBuffer(_device, &bufferInfo);
        if (_resourceBuffer == nullptr)
        {
            Debug::Log(Fmt("Failed to create buffer `{}`: {}", name, SDL_GetError()), Debug::LogLevel::Error);
        }
        else
        {
            SDL_SetGPUBufferName(_device, _resourceBuffer, name.c_str());
        }
        
        // Create transfer buffer.
        auto transferBufferInfo = SDL_GPUTransferBufferCreateInfo
        {
            .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
            .size  = capacity * sizeof(T)
        };
        _transferBuffer = SDL_CreateGPUTransferBuffer(_device, &transferBufferInfo);
        if (_transferBuffer == nullptr)
        {
            Debug::Log(Fmt("Failed to create transfer buffer `{}`: {}", name, SDL_GetError()), Debug::LogLevel::Error);
        }
    }

    template <typename T>
    void Buffer<T>::Update(SDL_GPUCopyPass& copyPass, std::span<const T> data, int offset)
    {
        if (!IsValid())
        {
            Debug::Log(Fmt("Attempted to update invalid GPU buffer `{}`.", _name), Debug::LogLevel::Warning);
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
            .offset = offset * sizeof(T),
            .size   = (uint)data.size_bytes()
        };
        SDL_UploadToGPUBuffer(&copyPass, &transferBufferLoc, &bufferRegion, true);
    }

    template <typename T>
    void Buffer<T>::Bind(SDL_GPURenderPass& renderPass, int offset)
    {
        if (!IsValid())
        {
            Debug::Log(Fmt("Attempted to bind invalid GPU buffer `{}`.", _name), Debug::LogLevel::Warning);
            return;
        }

        auto bufferBindings = SDL_GPUBufferBinding
        {
            .buffer = _resourceBuffer,
            .offset = offset * sizeof(T)
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
            Debug::Log(Fmt("Attempted to bind indirect GPU buffer `{}`.", _name), Debug::LogLevel::Info);
        }
    }

    template <typename T>
    bool Buffer<T>::IsValid() const
    {
        return _resourceBuffer != nullptr && _transferBuffer != nullptr;
    }
};
