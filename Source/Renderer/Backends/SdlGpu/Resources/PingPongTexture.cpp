#include "Framework.h"
#include "Renderer/Backends/SdlGpu/Resources/PingPongTexture.h"

namespace Silent::Renderer
{
    PingPongTexture::~PingPongTexture()
    {
        Release();
    }

    void PingPongTexture::Initialize(SDL_GPUDevice& device)
    {
        _device = &device;
    }

    void PingPongTexture::Swap()
    {
        _writeIdx = 1 - _writeIdx;
    }

    void PingPongTexture::Release()
    {
        if (_device == nullptr)
        {
            return;
        }

        for (auto* target : _targets)
        {
            SDL_ReleaseGPUTexture(_device, target);
            target = nullptr;
        }
    }

    SDL_GPUTexture*& PingPongTexture::Read()
    {
        return _targets[1 - _writeIdx];
    }

    SDL_GPUTexture*& PingPongTexture::Write()
    {
        return _targets[_writeIdx];
    }
}
