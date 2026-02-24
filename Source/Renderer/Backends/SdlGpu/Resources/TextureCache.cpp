#include "Framework.h"
#include "Renderer/Backends/SdlGpu/Resources/TextureCache.h"

#include "Application.h"
#include "Assets/AssetStreamer.h"
#include "Renderer/Common/Constants.h"
#include "Renderer/Common/Resources/TextureCache.h"
#include "Utils/Utils.h"

using namespace Silent::Assets;
using namespace Silent::Utils;

namespace Silent::Renderer::SdlGpu
{
    Texture::Texture(SDL_GPUDevice& device, SDL_GPUCopyPass& copyPass,
                     SDL_GPUTextureUsageFlags usageFlags, std::span<const byte> pixels, const Vector2i& res,
                     const std::string& name)
    {
        _device     = &device;
        _resolution = res;

        // Create texture.
        auto texInfo = SDL_GPUTextureCreateInfo
        {
            .type                 = SDL_GPU_TEXTURETYPE_2D,
            .format               = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
            .usage                = usageFlags,
            .width                = (uint)res.x,
            .height               = (uint)res.y,
            .layer_count_or_depth = 1,
            .num_levels           = 1
        };
        _texture = SDL_CreateGPUTexture(_device, &texInfo);
        SDL_SetGPUTextureName(_device, _texture, name.c_str());

        // Create transfer buffer.
        auto transferBufferInfo = SDL_GPUTransferBufferCreateInfo
        {
            .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
            .size  = (uint)((res.x * res.y) * RGBA_COMP_COUNT)
        };
        auto* transferBuffer = SDL_CreateGPUTransferBuffer(_device, &transferBufferInfo);

        byte* mappedTransferData = (byte*)SDL_MapGPUTransferBuffer(_device, transferBuffer, false);
        memcpy(mappedTransferData, pixels.data(), (res.x * res.y) * RGBA_COMP_COUNT);
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

    Texture::~Texture()
    {
        SDL_ReleaseGPUTexture(_device, _texture);
    }

    SDL_GPUTexture* Texture::GetHandle()
    {
        return _texture;
    }

    const Vector2i& Texture::GetResolution() const
    {
        return _resolution;
    }

    void Texture::Update(SDL_GPUCopyPass& copyPass,
                         std::span<const byte> pixels, const Vector2i& region, const Vector2i& size)
    {
        // Create transfer buffer.
        auto transferBufferInfo = SDL_GPUTransferBufferCreateInfo
        {
            .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
            .size  = (uint)((size.x * size.y) * RGBA_COMP_COUNT)
        };
        auto* transferBuffer = SDL_CreateGPUTransferBuffer(_device, &transferBufferInfo);

        byte* mappedTransferData = (byte*)SDL_MapGPUTransferBuffer(_device, transferBuffer, false);
        memcpy(mappedTransferData, pixels.data(), (size.x * size.y) * RGBA_COMP_COUNT);
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

    void Texture::Bind(SDL_GPURenderPass& renderPass, SDL_GPUSampler& sampler, int slotIdx)
    {
        auto texSamplerBinding = SDL_GPUTextureSamplerBinding
        {
            .texture = _texture,
            .sampler = &sampler
        };
        SDL_BindGPUFragmentSamplers(&renderPass, slotIdx, &texSamplerBinding, 1);
    }

    TextureCache::TextureCache(SDL_GPUDevice& device)
    {
        _device = &device;
    }

    void TextureCache::Upload(SDL_GPUCopyPass& copyPass,
                              std::span<const byte> pixels, const Vector2i res, const std::string& name)
    {
        // Check if texture with same name already exists.
        if (Find(_textures, name) != nullptr)
        {
            Debug::Log(Fmt("Attempted to overwrite existing GPU texture `{}`.", name), Debug::LogLevel::Warning);
            return;
        }

        _textures[name] = std::make_unique<Texture>(*_device, copyPass,
                                                    SDL_GPU_TEXTUREUSAGE_SAMPLER, pixels, res,
                                                    name);
    }

    void TextureCache::Upload(SDL_GPUCopyPass& copyPass, const std::string& assetName)
    {
        auto& assets = g_App.GetAssets();

        // Get asset.
        const auto* asset = assets.GetAsset(assetName);
        if (asset == nullptr)
        {
            Debug::Log(Fmt("Attempted to load invalid asset `{}` as GPU texture.", asset->Name),
                       Debug::LogLevel::Error);
        }

        // Load image asset.
        switch (asset->Type)
        {
            case AssetType::Png:
            {
                UploadPng(copyPass, *asset);
                break;
            }
            case AssetType::Tim:
            {
                UploadTim(copyPass, *asset);
                break;
            }
            default:
            {
                Debug::Log(Fmt("Attempted to load non-image asset `{}` as GPU texture.", asset->Name),
                           Debug::LogLevel::Error);
                break;
            }
        }
    }

    void TextureCache::Release(const std::string& name)
    {
        _textures.erase(name);
    }

    void TextureCache::Clear()
    {
        _textures.clear();
    }

    Texture* TextureCache::operator[](const std::string& name)
    {
        auto* tex = Find(_textures, name);
        if (tex == nullptr)
        {
            Debug::Log(Fmt("Attempted to get missing GPU texture `{}`.", name), Debug::LogLevel::Warning);
            return nullptr;
        }

        return (Texture*)tex->get();
    }

    void TextureCache::UploadPng(SDL_GPUCopyPass& copyPass, const Asset& asset)
    {
        const auto data = asset.GetData<PngAsset>();
        Upload(copyPass, ToSpan(data->Pixels), data->Resolution, asset.Name);
    }

    void TextureCache::UploadTim(SDL_GPUCopyPass& copyPass, const Asset& asset)
    {
        const auto data = asset.GetData<TimAsset>();
        Upload(copyPass, ToSpan(data->Pixels), data->Resolution, asset.Name);
    }
}
