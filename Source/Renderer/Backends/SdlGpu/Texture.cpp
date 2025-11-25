#include "Framework.h"
#include "Renderer/Backends/SdlGpu/Texture.h"

#include "Application.h"
#include "Assets/Assets.h"
#include "Utils/Utils.h"

using namespace Silent::Assets;
using namespace Silent::Utils;

namespace Silent::Renderer
{
    Texture::Texture(SDL_GPUDevice& device, SDL_GPUCopyPass& copyPass, const std::span<byte>& pixels, const Vector2i& res, const std::string& name)
    {
        Initialize(device, copyPass, pixels, res, name);
    }

    Texture::~Texture()
    {
        SDL_ReleaseGPUTexture(_device, _texture);
    }

    void Texture::Initialize(SDL_GPUDevice& device, SDL_GPUCopyPass& copyPass, const std::span<byte>& pixels, const Vector2i res, const std::string& name)
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

    void Texture::Update(SDL_GPUCopyPass& copyPass, const std::span<byte>& pixels, const Vector2i& region, const Vector2i& size)
    {
        Debug::Assert(_device != nullptr, "Attempted to update uninitialized GPU texture.");

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

    void Texture::Bind(SDL_GPURenderPass& renderPass, SDL_GPUSampler& sampler)
    {
        Debug::Assert(_device != nullptr, "Attempted to bind uninitialized GPU texture.");

        auto texSamplerBinding = SDL_GPUTextureSamplerBinding
        {
            .texture = _texture,
            .sampler = &sampler
        };
        SDL_BindGPUFragmentSamplers(&renderPass, 0, &texSamplerBinding, 1);
    }

    TextureManager::~TextureManager()
    {
        _textures.clear();
    }

    Texture* TextureManager::Get(const std::string& name)
    {
        Debug::Assert(_device != nullptr, "Attempted to get GPU texture from uninitialized texture manager.");

        auto* tex = Find(_textures, name);
        if (tex == nullptr)
        {
            Debug::Log(Fmt("Attempted to get invalid GPU texture `{}`.", name));
        }

        return tex;
    }

    void TextureManager::Initialize(SDL_GPUDevice& device)
    {
        _device = &device;
    }

    void TextureManager::Load(SDL_GPUCopyPass& copyPass, const std::span<byte>& pixels, const Vector2i res, const std::string& name)
    {
        Debug::Assert(_device != nullptr, "Attempted to load GPU texture in uninitialized texture manager.");

        if (Find(_textures, name) != nullptr)
        {
            Debug::Log(Fmt("Attempted to load already loaded GPU texture `{}`.", name));
            return;
        }
        
        Debug::Log(Fmt("Loaded GPU texture `{}`.", name));

        // @todo Can't emplace????
        //_textures.emplace("aaa", Texture(*_device, copyPass, pixels, res, "aaa"));
        _textures.emplace("aaa", Texture());
        _textures["aaa"].Initialize(*_device, copyPass, pixels, res, "aaa");
    }

    void TextureManager::Load(SDL_GPUCopyPass& copyPass, int assetIdx)
    {
        Debug::Assert(_device != nullptr, "Attempted to load GPU texture in uninitialized texture manager.");
        
        auto& assets = g_App.GetAssets();

        // Get asset.
        const auto asset = assets.GetAsset(assetIdx);
        if (asset == nullptr)
        {
            Debug::Log(Fmt("Attempted to load GPU texture from invalid asset `{}`.", assetIdx), Debug::LogLevel::Warning);
            return;
        }

        // Check if asset is TIM image.
        if (asset->Type != AssetType::Tim)
        {
            Debug::Log(Fmt("Attempted to load GPU texture from non-image asset `{}`.", asset->Name), Debug::LogLevel::Warning);
            return;
        }

        // Initialize TIM image texture.
        auto data = asset->GetData<TimAsset>();
        auto tex = Texture(*_device, copyPass, ToSpan(data->Pixels), data->Resolution, asset->Name);
        Load(copyPass, ToSpan(data->Pixels), data->Resolution, asset->Name);
    }
}
