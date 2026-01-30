#include "Framework.h"
#include "Renderer/Backends/SdlGpu/Resources/IndirectBuffer.h"

#include "Renderer/Backends/SdlGpu/Resources/Buffer.h"
#include "Utils/Utils.h"

using namespace Silent::Utils;

namespace Silent::Renderer::SdlGpu
{
    IndirectBuffer::IndirectBuffer(SDL_GPUDevice& device, SDL_GPUCopyPass& copyPass, const std::vector<SDL_GPUIndexedIndirectDrawCommand>& commands,
                                   const std::string& name)
    {
        _commandBuffer.Initialize(device, SDL_GPU_BUFFERUSAGE_INDIRECT, commands.size(), name);
        _commandBuffer.Update(copyPass, ToSpan(commands), 0);
    }

    const std::string& IndirectBuffer::GetName() const
    {
        return _commandBuffer.GetName();
    }

    void IndirectBuffer::Draw(SDL_GPURenderPass& renderPass)
    {
        // @todo
    }
}
