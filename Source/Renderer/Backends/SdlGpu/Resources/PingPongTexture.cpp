#include "Framework.h"
#include "Renderer/Backends/SdlGpu/Resources/PingPongTexture.h"

namespace Silent::Renderer
{
    SDL_GPUTexture*& PingPongTexture::Read()
    {
        return _targets[1 - _writeIdx];
    }

    SDL_GPUTexture*& PingPongTexture::Write()
    {
        return _targets[_writeIdx];
    }

    void PingPongTexture::Initialize(SDL_GPUDevice& device)
    {
        _device = &device;
    }

    void PingPongTexture::Release()
    {
        if (_device == nullptr)
        {
            return;
        }

        for (auto* target : _targets)
        {
            if (target == nullptr)
            {
                continue;
            }

            SDL_ReleaseGPUTexture(_device, target);
            target = nullptr;
        }
    }
}
