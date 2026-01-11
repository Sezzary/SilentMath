#pragma once

#include "Renderer/Backends/SdlGpu/Buffer/Buffer.h"

namespace Silent::Renderer
{
    /** @brief GPU indirect buffer. */
    class IndirectBuffer
    {
    private:
        // =======
        // Fields
        // =======

        Buffer<SDL_GPUIndexedIndirectDrawCommand> _commandBuffer = {};

    public:
        // =============
        // Constructors
        // =============

        IndirectBuffer(SDL_GPUDevice& device, SDL_GPUCopyPass& copyPass, const std::vector<SDL_GPUIndexedIndirectDrawCommand>& commands,
                       const std::string& name = {});

        // ========
        // Getters
        // ========

        /** @brief Gets the name of the GPU buffer.
         *
         * @return GPU buffer name.
         */
        const std::string& GetName() const;

        // @todo
        void Draw(SDL_GPURenderPass& renderPass);
    };

    const std::string& IndirectBuffer::GetName() const
    {
        return _vertexBuffer.GetName();
    }
}
