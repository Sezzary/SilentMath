#pragma once

#include "Renderer/Backends/SdlGpu/Buffer/Buffer.h"

namespace Silent::Renderer
{
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

        IndirectBuffer(SDL_GPUDevice& device, const std::vector<SDL_GPUIndexedIndirectDrawCommand>& commands, const std::string& name = {})
        {
            _commandBuffer = Buffer<SDL_GPUIndexedIndirectDrawCommand>(device, SDL_GPU_BUFFERUSAGE_INDIRECT, commands.size());
            _commandBuffer.Update();
        }

        void Draw(SDL_GPURenderPass& renderPass);
    };
}
