#include "Framework.h"
#include "Renderer/Backends/SdlGpu/Texture.h"

#include "Application.h"
#include "Assets/Assets.h"
#include "Utils/Utils.h"

using namespace Silent::Assets;
using namespace Silent::Utils;

namespace Silent::Renderer
{
    SdlGpuTexture::SdlGpuTexture(SDL_GPUDevice& device, SDL_GPUCopyPass& copyPass, const std::span<byte>& pixels, const Vector2i res, const std::string& name)
    {
        _device = &device;

        // Create texture.
        auto texInfo = SDL_GPUTextureCreateInfo
        {
            .type                 = SDL_GPU_TEXTURETYPE_2D,
            .format               = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
            .usage                = SDL_GPU_TEXTUREUSAGE_SAMPLER,
            .width                = (uint)res.x,
            .height               = (uint)res.y,
            .layer_count_or_depth = 1,
            .num_levels           = 1
        };
        _texture = SDL_CreateGPUTexture(_device, &texInfo);

        // Set texture name.
        SDL_SetGPUTextureName(_device, _texture, name.c_str());

        // Create transfer buffer.
        auto transferBufferInfo = SDL_GPUTransferBufferCreateInfo
        {
            .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
            .size  = (uint)((res.x * res.y) * 4)
        };
        auto* transferBuffer = SDL_CreateGPUTransferBuffer(_device, &transferBufferInfo);

        byte* mappedTransferData = (byte*)SDL_MapGPUTransferBuffer(_device, transferBuffer, false);
        memcpy(mappedTransferData, pixels.data(), (res.x * res.y) * 4);
        SDL_UnmapGPUTransferBuffer(_device, transferBuffer);

        // Upload texture data.
        auto texTransferInfo = SDL_GPUTextureTransferInfo
        {
            .transfer_buffer = transferBuffer,
            .offset          = 0
        };
        auto texRegion = SDL_GPUTextureRegion
        {
            .texture = _texture,
            .w       = (uint)res.x,
            .h       = (uint)res.y,
            .d       = 1
        };
        SDL_UploadToGPUTexture(&copyPass, &texTransferInfo, &texRegion, false);

        // Free GPU resources.
        SDL_ReleaseGPUTransferBuffer(_device, transferBuffer);
    }

    SdlGpuTexture::~SdlGpuTexture()
    {
        SDL_ReleaseGPUTexture(_device, _texture);
    }

    void SdlGpuTexture::Update(SDL_GPUCopyPass& copyPass, const std::span<byte>& pixels, const Vector2i& region, const Vector2i& size)
    {
        // Create transfer buffer.
        auto transferBufferInfo = SDL_GPUTransferBufferCreateInfo
        {
            .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
            .size  = (uint)((size.x * size.y) * 4)
        };
        auto* transferBuffer = SDL_CreateGPUTransferBuffer(_device, &transferBufferInfo);

        byte* mappedTransferData = (byte*)SDL_MapGPUTransferBuffer(_device, transferBuffer, false);
        memcpy(mappedTransferData, pixels.data(), (size.x * size.y) * 4);
        SDL_UnmapGPUTransferBuffer(_device, transferBuffer);

        // Upload texture data.
        auto texTransferInfo = SDL_GPUTextureTransferInfo
        {
            .transfer_buffer = transferBuffer,
            .offset          = 0
        };
        auto texRegion = SDL_GPUTextureRegion
        {
            .texture = _texture,
            .x       = (uint)region.x,
            .y       = (uint)region.y,
            .w       = (uint)size.x,
            .h       = (uint)size.y,
            .d       = 1
        };
        SDL_UploadToGPUTexture(&copyPass, &texTransferInfo, &texRegion, false);

        // Free GPU resources.
        SDL_ReleaseGPUTransferBuffer(_device, transferBuffer);
    }

    void SdlGpuTexture::Bind(SDL_GPURenderPass& renderPass, SDL_GPUSampler& sampler)
    {
        auto texSamplerBinding = SDL_GPUTextureSamplerBinding
        {
            .texture = _texture,
            .sampler = &sampler
        };
        SDL_BindGPUFragmentSamplers(&renderPass, 0, &texSamplerBinding, 1);
    }

    SdlGpuTextureManager::SdlGpuTextureManager(SDL_GPUDevice& device)
    {
        _device = &device;
    }

    SdlGpuTexture* SdlGpuTextureManager::Get(const std::string& name)
    {
        auto* tex = Find(_textures, name);
        if (tex == nullptr)
        {
            Debug::Log(Fmt("Texture manager attempted to get missing GPU texture `{}`.", name));
            return nullptr;
        }

        return (SdlGpuTexture*)tex->get();
    }

    void SdlGpuTextureManager::Load(SDL_GPUCopyPass& copyPass, const std::span<byte>& pixels, const Vector2i res, const std::string& name)
    {
        _textures[name] = std::make_unique<SdlGpuTexture>(*_device, copyPass, pixels, res, name);
    }

    void SdlGpuTextureManager::Load(SDL_GPUCopyPass& copyPass, const std::string& assetName)
    {
        auto& assets = g_App.GetAssets();

        // Get asset.
        const auto asset = assets.GetAsset(assetName);
        if (asset == nullptr)
        {
            throw std::runtime_error(Fmt("Attempted to load invalid asset `{}` as GPU texture.", assetName));
        }

        // Check if asset is TIM image.
        if (asset->Type != AssetType::Tim)
        {
            throw std::runtime_error(Fmt("Attempted to load non-image asset `{}` as GPU texture.", asset->Name));
        }

        // Initialize TIM image texture.
        auto data = asset->GetData<TimAsset>();
        Load(copyPass, ToSpan(data->Pixels), data->Resolution, asset->Name);
    }
}
