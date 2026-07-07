#include "Framework.h"
#include "Renderer/Backends/SdlGpu/SdlGpu.h"

#include "Application.h"
#include "Renderer/Backends/SdlGpu/Pipeline/Pipeline.h"
#include "Renderer/Backends/SdlGpu/Resources/MeshCache.h"
#include "Renderer/Backends/SdlGpu/Resources/PingPongTexture.h"
#include "Renderer/Backends/SdlGpu/Resources/TextureCache.h"
#include "Renderer/Common/Resources/Buffers.h"
#include "Renderer/Common/Resources/Uniforms.h"
#include "Renderer/Common/Utils.h"
#include "Renderer/Renderer.h"
#include "Services/Filesystem.h"
#include "Services/Options.h"
#include "Services/Platform.h"
#include "Utils/Utils.h"

using namespace Silent::Assets;
using namespace Silent::Services;
using namespace Silent::Utils;

namespace Silent::Renderer::SdlGpu
{
    void Renderer::Initialize(SDL_Window& window)
    {
        static constexpr char NAME[] = "SDL_gpu";

        auto& assets = g_App.GetAssets();

        // @todo Make function for common init stuff to call at the start of every backend-specific init function.

        _type   = RendererType::SdlGpu;
        _window = &window;

        // Define format flags.
        int formatFlags = 0;
        switch (PLATFORM_TYPE)
        {
            case PlatformType::Windows:
            {
                formatFlags = SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL;
                break;
            }
            case PlatformType::MacOs:
            {
                formatFlags = SDL_GPU_SHADERFORMAT_MSL;
                break;
            }
            case PlatformType::Linux:
            {
                formatFlags = SDL_GPU_SHADERFORMAT_SPIRV;
                break;
            }
        }

        // Create GPU device.
        _device = SDL_CreateGPUDevice(formatFlags, Debug::IS_DEBUG_BUILD, nullptr);
        if (_device == nullptr)
        {
            throw std::runtime_error(Fmt("Failed to create GPU device for {} renderer: {}", NAME, SDL_GetError()));
        }
        Debug::Log(Fmt("Using {} renderer with {} backend.", NAME, SDL_GetGPUDeviceDriver(_device)));

        // Claim window.
        if (!SDL_ClaimWindowForGPUDevice(_device, _window))
        {
            throw std::runtime_error(Fmt("Failed to claim window for GPU device: {}", SDL_GetError()));
        }

        // Enable VSync.
        bool supportsMailbox = SDL_WindowSupportsGPUPresentMode(_device, _window, SDL_GPU_PRESENTMODE_MAILBOX);
        SDL_SetGPUSwapchainParameters(_device, _window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
                                      supportsMailbox ? SDL_GPU_PRESENTMODE_MAILBOX : SDL_GPU_PRESENTMODE_VSYNC);

        // Initialize buffers.
        InitializeDoubleBuffer();
        InitializeGpuBuffers();

        // Initialize GPU textures.
        _textures = std::make_unique<TextureCache>(*_device);
        _renderTexture.Initialize(*_device);

        // Initialize pipelines.
        _pipelines.Initialize(*_window, *_device);

        // Create nearest-neighbor sampler.
        auto nearestSamplerInfo = SDL_GPUSamplerCreateInfo
        {
            .min_filter     = SDL_GPU_FILTER_NEAREST,
            .mag_filter     = SDL_GPU_FILTER_NEAREST,
            .mipmap_mode    = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
            .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
            .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
            .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT
        };
        _samplers.push_back(SDL_CreateGPUSampler(_device, &nearestSamplerInfo));

        // Create linear sampler.
        auto linearSamplerInfo = SDL_GPUSamplerCreateInfo
        {
            .min_filter     = SDL_GPU_FILTER_LINEAR,
            .mag_filter     = SDL_GPU_FILTER_LINEAR,
            .mipmap_mode    = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
            .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
            .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
            .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT
        };
        _samplers.push_back(SDL_CreateGPUSampler(_device, &linearSamplerInfo));

        // Create ImGui context.
        ImGui::CreateContext();
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad;
        ImGui_ImplSDL3_InitForSDLGPU(_window);

        // Initialize ImGui backend.
        auto initInfo = ImGui_ImplSDLGPU3_InitInfo
        {
            .Device            = _device,
            .ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(_device, _window),
            .MSAASamples       = SDL_GPU_SAMPLECOUNT_1
        };
        ImGui_ImplSDLGPU3_Init(&initInfo);

        // Upload transfer data to GPU resources.
        auto* uploadCmdBuffer = SDL_AcquireGPUCommandBuffer(_device);
        auto* copyPass        = SDL_BeginGPUCopyPass(uploadCmdBuffer);

        // @temp
        //GetMeshes().Upload(*copyPass, "CHARA/DOC.ILM");
        //GetMeshes().Upload(*copyPass, "ITEM/UNQE1.TMD");

        // Load temp. textures.
        //GetTextures().Upload(*copyPass, "TIM/HERO_PIC.TIM");
        //GetTextures().Upload(*copyPass, "1ST/2ZANKO_E.TIM");
        assets.Load("TIM/BG_ETC.TIM");
        
        GetTextures().Upload(*copyPass, ToSpan(DEFAULT_TEXTURE_PIXELS), DEFAULT_TEXTURE_RES, "");
        // @todo If atlas textures aren't updated and the texture is missing, for some reason
        // the app hangs instead of crashing like it's supposed to. Why isn't such an error
        // handled as written?
        UpdateResources(*copyPass);

        SDL_EndGPUCopyPass(copyPass);
        SDL_SubmitGPUCommandBuffer(uploadCmdBuffer);
    }

    void Renderer::Deinitialize()
    {
        // @todo Errors.

        SDL_WaitForGPUIdle(_device);

        ImGui_ImplSDL3_Shutdown();
        ImGui_ImplSDLGPU3_Shutdown();
        ImGui::DestroyContext();

        _gpuBuffers = {};
        _pipelines.Deinitialize();

        SDL_ReleaseWindowFromGPUDevice(_device, _window);
        SDL_DestroyGPUDevice(_device);
    }

    void Renderer::PrepareFrameResources()
    {
        // Acquire command buffer.
        _commandBuffer = SDL_AcquireGPUCommandBuffer(_device);
        if (_commandBuffer == nullptr)
        {
            Debug::Log(Fmt("Failed to acquire command buffer: {}", SDL_GetError()), Debug::LogLevel::Error);
            return;
        }

        // Acquire render textures.
        UpdateRenderTargets();

        // Acquire depth stencil texture.
        _depthTexture = GetDepthTexture();
        if (_depthTexture == nullptr)
        {
            return;
        }

        // Acquire swapchain texture.
        _swapchainTexture = nullptr;
        if (!SDL_WaitAndAcquireGPUSwapchainTexture(_commandBuffer, _window, &_swapchainTexture, nullptr, nullptr))
        {
            return;
        }

        // Process copy pass.
        auto* copyPass = SDL_BeginGPUCopyPass(_commandBuffer);

        UpdateResources(*copyPass);

        SDL_EndGPUCopyPass(copyPass);
    }

    void Renderer::Update()
    {
        // Frame setup.
        SortRenderBufferData();
        ClearDrawBatches();

        // Draw frame.
        if (_swapchainTexture != nullptr)
        {
            DrawFrame();
        }

        // Submit command buffer to GPU.
        SDL_SubmitGPUCommandBuffer(_commandBuffer);
    }

    void Renderer::SaveScreenshot() const
    {
        constexpr int COLOR_CHANNEL_COUNT = 3; // RGB.

        // @todo Doesn't work.

        const auto& fs = g_App.GetFilesystem();

        // Get window size.
        auto res = GetViewportResolution();

        // Ensure directory exists.
        auto timestamp = GetCurrentDateString() + "_" + GetCurrentTimeString();
        auto filename  = (SCREENSHOT_FILENAME_BASE + timestamp) + PNG_FILE_EXT;
        auto path      = fs.GetScreenshotsDirectory() / filename;
        stdfs::create_directories(path.parent_path());

        // Get window surface.
        auto* surface = SDL_GetWindowSurface(_window);
        if (surface == nullptr)
        {
            Debug::Log(Fmt("Failed to save screenshot: {}", SDL_GetError()),
                       Debug::LogLevel::Warning, Debug::LogMode::All, true);
            return;
        }

        // Lock surface to access pixels.
        if (!SDL_LockSurface(surface))
        {
            Debug::Log(Fmt("Failed to save screenshot: {}", SDL_GetError()),
                       Debug::LogLevel::Warning, Debug::LogMode::All, true);
            return;
        }

        // Write screenshot file.
        if (stbi_write_png(path.string().c_str(), res.x, res.y, COLOR_CHANNEL_COUNT,
                           surface->pixels, res.x * COLOR_CHANNEL_COUNT))
        {
            Debug::Log("Saved screenshot.", Debug::LogLevel::Info, Debug::LogMode::All, true);
        }
        else
        {
            Debug::Log("Failed to save screenshot.", Debug::LogLevel::Warning, Debug::LogMode::All, true);
        }

        SDL_UnlockSurface(surface);
    }

    TextureCache& Renderer::GetTextures()
    {
        return *(TextureCache*)_textures.get();
    }

    MeshCache& Renderer::GetMeshes()
    {
        return *(MeshCache*)_meshes.get();
    }

    void Renderer::UpdateRenderTargets()
    {
        if (_renderTexture.Read()  != nullptr &&
            _renderTexture.Write() != nullptr &&
            !_doubleBuffer.Render.IsResized)
        {
            return;
        }

        _renderTexture.Release();

        auto viewportRes   = GetViewportResolution();
        auto renderTexInfo = SDL_GPUTextureCreateInfo
        {
            .type                 = SDL_GPU_TEXTURETYPE_2D,
            .format               = SDL_GetGPUSwapchainTextureFormat(_device, _window),
            .usage                = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER,
            .width                = (uint)viewportRes.x,
            .height               = (uint)viewportRes.y,
            .layer_count_or_depth = 1,
            .num_levels           = 1
        };
        _renderTexture.Read()  = SDL_CreateGPUTexture(_device, &renderTexInfo);
        _renderTexture.Write() = SDL_CreateGPUTexture(_device, &renderTexInfo);
    }

    SDL_GPUTexture* Renderer::GetDepthTexture()
    {
        if (_depthTexture != nullptr && !_doubleBuffer.Render.IsResized)
        {
            return _depthTexture;
        }

        if (_depthTexture != nullptr)
        {
            SDL_ReleaseGPUTexture(_device, _depthTexture);
        }

        auto viewportRes  = GetViewportResolution();
        auto depthTexInfo = SDL_GPUTextureCreateInfo
        {
            .type                 = SDL_GPU_TEXTURETYPE_2D,
            .format               = SDL_GPU_TEXTUREFORMAT_D32_FLOAT,
            .usage                = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
            .width                = (uint)viewportRes.x,
            .height               = (uint)viewportRes.y,
            .layer_count_or_depth = 1,
            .num_levels           = 1
        };
        return SDL_CreateGPUTexture(_device, &depthTexInfo);
    }

    SDL_GPUSampler& Renderer::GetActiveSampler()
    {
        const auto& options = g_App.GetOptions();

        return *_samplers[(int)options->TextureFilter];
    }
}
