#include "Framework.h"
#include "Renderer/Backends/SdlGpu/SdlGpu.h"

#include "Application.h"
#include "Assets/Fonts.h"
#include "Renderer/Backends/SdlGpu/Pipeline/Pipeline.h"
#include "Renderer/Backends/SdlGpu/Resources/MeshCache.h"
#include "Renderer/Backends/SdlGpu/Resources/PingPongTexture.h"
#include "Renderer/Backends/SdlGpu/Resources/TextureCache.h"
#include "Renderer/Common/Resources/Buffers.h"
#include "Renderer/Common/Resources/Uniforms.h"
#include "Renderer/Common/Utils.h"
#include "Renderer/Common/View.h"
#include "Renderer/Renderer.h"
#include "Services/Filesystem.h"
#include "Services/Options.h"
#include "Services/Platform.h"
#include "Utils/Utils.h"
#include "Utils/Video.h"

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

        // Reset resized signal.
        _isResized = false;
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
        std::filesystem::create_directories(path.parent_path());

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
        if (_renderTexture.Read() != nullptr && _renderTexture.Write() != nullptr &&
            !_isResized)
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
        if (_depthTexture != nullptr && !_isResized)
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

    void Renderer::Draw3dScene()
    {
        // Process copy pass.
        auto* copyPass = SDL_BeginGPUCopyPass(_commandBuffer);

        CopyImmediatePrimitives3d(*copyPass);

        SDL_EndGPUCopyPass(copyPass);

        // Process render pass.
        auto colorTargetInfo = SDL_GPUColorTargetInfo
        {
            .texture     = _renderTexture.Read(),
            .clear_color = SDL_FColor{ _clearColor.R(), _clearColor.G(), _clearColor.B(), _clearColor.A() },
            .load_op     = SDL_GPU_LOADOP_CLEAR,
            .store_op    = SDL_GPU_STOREOP_STORE
        };
        auto depthTargetInfo = SDL_GPUDepthStencilTargetInfo
        {
            .texture     = _depthTexture,
            .clear_depth = 1.0f,
            .load_op     = SDL_GPU_LOADOP_CLEAR,
            .store_op    = SDL_GPU_STOREOP_DONT_CARE
        };
        auto& renderPass = *SDL_BeginGPURenderPass(_commandBuffer, &colorTargetInfo, 1, &depthTargetInfo);

        //GetMeshes().Bind(renderPass);
        //_pipelines.Bind(renderPass, RenderStage::Model, BlendMode::Opaque);

        _view.Move();

        // @temp
        //---------------------------

        //auto* tex = GetTextures()[g_App.GetVideo().GetName()];
        //if (tex == nullptr)
        //{
        //    auto* tex = GetTextures()["TIM/BG_ETC.TIM"];
        //}

        /*if (tex != nullptr)
        {
            tex->Bind(renderPass, GetActiveSampler());
            
            _view.Move();

            auto model = Matrix::Identity;
            model.Rotate(DEG_TO_RAD(45.0f), Vector3::UnitX);

            auto viewProj = _view.GetMatrix(glm::radians(45.0f), GetViewportAspectRatio(), 0.1f, 100.0f);

            auto uni0 = UniformView{};
            auto uni1 = UniformPrimitive3d{};
            memcpy(&uni0.ViewProjMat, &viewProj[0][0], 64);
            memcpy(&uni1.ModelMat, &model[0][0], 64);
            PushVertexUniform(uni0, 0);
            PushVertexUniform(uni1, 1);

            // Push uniform.
            auto uni = UniformModel
            {
                .IsFastAlpha = false
            };
            PushFragmentUniform(uni, 0);

            // Draw.
            //const auto* mesh = GetMeshes()["TestCube"];
            //if (mesh != nullptr && mesh->IsValid())
            //{
            //    SDL_DrawGPUIndexedPrimitives(&renderPass, mesh->IdxCount, 1, mesh->IdxOffset, mesh->VertexOffset, 0);
            //    _doubleBuffer.Active.DrawCallCount++;
            //}
        }*/

        //---------------------------

        _gpuBuffers.ImmediateVertices3d.Bind(renderPass, 0, 0);
        //auto* tex = GetTextures()["TIM/BG_ETC.TIM"];
        
        // Draw 3D primitives.
        for (const auto& batch : _drawBatches.Primitives3d)
        {
            // Bind pipeline.
            _pipelines.Bind(renderPass, batch.RenderStg, batch.BlendMd);
            PushFragmentUniform(batch.Uniform, 0);

            auto model = Matrix::Identity;
            model.Rotate(DEG_TO_RAD(45.0f), Vector3::UnitX);

            auto viewProj = _view.GetMatrix(glm::radians(45.0f), GetViewportAspectRatio(), 0.1f, 100.0f);

            auto uni0 = UniformView{};
            memcpy(&uni0.ViewProjMat, &viewProj[0][0], 64);
            PushVertexUniform(uni0, 0);

            auto uni1 = UniformPrimitive3d{};
            memcpy(&uni1.ModelMat, &model[0][0], 64);
            PushVertexUniform(uni1, 1);

            // Bind texture.
            auto* tex = GetTextures()[batch.TextureName];
            if (tex != nullptr)
            {
                tex->Bind(renderPass, GetActiveSampler());
            }

            // Draw.
            SDL_DrawGPUIndexedPrimitives(&renderPass, batch.VertexCount, 1, batch.IdxOffset, batch.VertexOffset, 0);
            _doubleBuffer.Active.DrawCallCount++;
        }

        SDL_EndGPURenderPass(&renderPass);
    }

    void Renderer::DrawDither()
    {
        auto& options = g_App.GetOptions();
        
        // Process copy pass.
        auto* copyPass = SDL_BeginGPUCopyPass(_commandBuffer);

        CopyGpuViewportQuad(*copyPass);

        SDL_EndGPUCopyPass(copyPass);

        // Process render pass.
        auto colorTargetInfo = SDL_GPUColorTargetInfo
        {
            .texture  = _renderTexture.Read(),
            .load_op  = SDL_GPU_LOADOP_LOAD,
            .store_op = SDL_GPU_STOREOP_STORE
        };
        auto& renderPass = *SDL_BeginGPURenderPass(_commandBuffer, &colorTargetInfo, 1, nullptr);

        // @todo Severe visual artefacts.
        // Dither.
        if (options->EnableDithering)
        {
            // Bind pipeline.
            _pipelines.Bind(renderPass, RenderStage::Dither, BlendMode::Opaque);

            // Bind render texture.
            auto binding = SDL_GPUTextureSamplerBinding
            {
                .texture = _renderTexture.Read(),
                .sampler = _samplers[(int)TextureFilterType::Nearest]
            };
            SDL_BindGPUFragmentSamplers(&renderPass, 0, &binding, 1);

            // Draw.
            SDL_DrawGPUIndexedPrimitives(&renderPass, QUAD_IDX_COUNT, 1, 0, 0, 0);
            _doubleBuffer.Active.DrawCallCount++;
        }

        SDL_EndGPURenderPass(&renderPass);
    }

    void Renderer::Draw2dScene()
    {
        // Process copy pass.
        auto* copyPass = SDL_BeginGPUCopyPass(_commandBuffer);

        CopyImmediatePrimitives2d(*copyPass);

        SDL_EndGPUCopyPass(copyPass);

        // Process render pass.
        auto colorTargetInfo = SDL_GPUColorTargetInfo
        {
            .texture  = _renderTexture.Read(),
            .load_op  = SDL_GPU_LOADOP_LOAD,
            .store_op = SDL_GPU_STOREOP_STORE
        };
        auto depthTargetInfo = SDL_GPUDepthStencilTargetInfo
        {
            .texture     = _depthTexture,
            .clear_depth = 1.0f,
            .load_op     = SDL_GPU_LOADOP_CLEAR,
            .store_op    = SDL_GPU_STOREOP_DONT_CARE
        };
        auto& renderPass = *SDL_BeginGPURenderPass(_commandBuffer, &colorTargetInfo, 1, &depthTargetInfo);

        // Draw 2D primitives.
        _gpuBuffers.ImmediateVertices2d.Bind(renderPass, 0, 0);
        for (const auto& batch : _drawBatches.Primitives2d)
        {
            // Bind pipeline.
            _pipelines.Bind(renderPass, batch.RenderStg, batch.BlendMd);
            PushFragmentUniform(batch.Uniform, 0);

            // Bind texture.
            if (!batch.TextureName.empty())
            {
                auto* tex = GetTextures()[batch.TextureName];
                if (tex != nullptr)
                {
                    tex->Bind(renderPass, GetActiveSampler());
                }
            }

            // Draw.
            SDL_DrawGPUIndexedPrimitives(&renderPass, batch.VertexCount, 1, batch.IdxOffset, batch.VertexOffset, 0);
            _doubleBuffer.Active.DrawCallCount++;
        }

        SDL_EndGPURenderPass(&renderPass);
    }

    void Renderer::DrawPostProcess()
    {
        const auto& options  = g_App.GetOptions();

        auto* copyPass = SDL_BeginGPUCopyPass(_commandBuffer);

        CopyGpuViewportQuad(*copyPass);

        SDL_EndGPUCopyPass(copyPass);

        auto RunPostProcessPass = [&](RenderStage renderStage, auto pushUniforms)
        {
            auto colorTargetInfo = SDL_GPUColorTargetInfo
            {
                .texture  = _renderTexture.Write(),
                .load_op  = SDL_GPU_LOADOP_DONT_CARE,
                .store_op = SDL_GPU_STOREOP_STORE
            };

            auto* renderPass = SDL_BeginGPURenderPass(_commandBuffer, &colorTargetInfo, 1, nullptr);

            _gpuBuffers.ViewportVertices.Bind(*renderPass, 0, 0);
            _pipelines.Bind(*renderPass, renderStage, BlendMode::Opaque);

            pushUniforms();

            auto binding = SDL_GPUTextureSamplerBinding
            {
                .texture = _renderTexture.Read(),
                .sampler = _samplers[(int)TextureFilterType::Nearest]
            };
            SDL_BindGPUFragmentSamplers(renderPass, 0, &binding, 1);

            SDL_DrawGPUIndexedPrimitives(renderPass, QUAD_IDX_COUNT, 1, 0, 0, 0);
            SDL_EndGPURenderPass(renderPass);

            _renderTexture.Swap();
            _doubleBuffer.Active.DrawCallCount++;
        };

        if (Debug::g_Work.BlendAlpha > 0.0f)
        {
            RunPostProcessPass(RenderStage::Fade, [&]()
            {
                auto uni = UniformLumaFade
                {
                    .FadeAlpha = Debug::g_Work.BlendAlpha,
                    .IsWhite   = false
                };
                PushFragmentUniform(uni, 0);
            });
        }

        if (options->EnableCrtFilter)
        {
            RunPostProcessPass(RenderStage::Crt, [&]()
            {
                auto uni = UniformCrt
                {
                    .Resolution = GetViewportResolution().ToVector2(),
                    .Time       = 0.0f
                };
                PushFragmentUniform(uni, 0);
            });
        }

        if (options->EnableVignette)
        {
            RunPostProcessPass(RenderStage::Vignette, [&]()
            {
                auto uni = UniformCrt
                {
                    .Resolution = GetViewportResolution().ToVector2(),
                    .Time       = 0.0f
                };
                PushFragmentUniform(uni, 0);
            });
        }
    }

    void Renderer::DrawViewport()
    {
        constexpr float BRIGHTNESS_STEP   = 0.25f / BRIGHTNESS_LEVEL_MAX;
        constexpr float BRIGHTNESS_MIDDLE = BRIGHTNESS_STEP * (BRIGHTNESS_LEVEL_MAX / 2);

        const auto& options = g_App.GetOptions();

        // Process render pass.
        auto colorTargetInfo = SDL_GPUColorTargetInfo
        {
            .texture     = _swapchainTexture,
            .clear_color = SDL_FColor{ Color::Black.R(), Color::Black.G(), Color::Black.B(), Color::Black.A() },
            .load_op     = SDL_GPU_LOADOP_CLEAR,
            .store_op    = SDL_GPU_STOREOP_STORE
        };
        auto& renderPass = *SDL_BeginGPURenderPass(_commandBuffer, &colorTargetInfo, 1, nullptr);

        // Bind viewport quad.
        _gpuBuffers.ViewportVertices.Bind(renderPass, 0, 0);

        _pipelines.Bind(renderPass, RenderStage::Blit, BlendMode::Opaque);

        // Push uniform.
        auto uni = UniformBlit
        {
            .Brightness = (options->BrightnessLevel * BRIGHTNESS_STEP) - BRIGHTNESS_MIDDLE
        };
        PushFragmentUniform(uni, 0);

        // Bind render texture.
        auto binding = SDL_GPUTextureSamplerBinding
        {
            .texture = _renderTexture.Read(),
            .sampler = _samplers[(int)TextureFilterType::Nearest]
        };
        SDL_BindGPUFragmentSamplers(&renderPass, 0, &binding, 1);

        // Draw.
        SDL_DrawGPUIndexedPrimitives(&renderPass, QUAD_IDX_COUNT, 1, 0, 0, 0);
        _doubleBuffer.Active.DrawCallCount++;

        SDL_EndGPURenderPass(&renderPass);
    }

    void Renderer::DrawPowerMenu()
    {
        // If power menu is disabled, return early.
        if (!Debug::g_Work.EnablePowerMenu)
        {
            return;
        }

        // Start new frame.
        ImGui_ImplSDLGPU3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        // Draw GUIs.
        for (auto& drawCall : _doubleBuffer.Render.DebugGuiDrawCalls)
        {
            drawCall();
        }

        // Prepare render data.
        ImGui::Render();
        auto* drawData = ImGui::GetDrawData();
        ImGui_ImplSDLGPU3_PrepareDrawData(drawData, _commandBuffer);

        // Process render pass.
        auto colorTargetInfo = SDL_GPUColorTargetInfo
        {
            .texture  = _swapchainTexture,
            .load_op  = SDL_GPU_LOADOP_LOAD,
            .store_op = SDL_GPU_STOREOP_STORE
        };
        auto* renderPass = SDL_BeginGPURenderPass(_commandBuffer, &colorTargetInfo, 1, nullptr);

        // Draw.
        ImGui_ImplSDLGPU3_RenderDrawData(drawData, _commandBuffer, renderPass);

        SDL_EndGPURenderPass(renderPass);
    }

    void Renderer::InitializeGpuBuffers()
    {
        constexpr int SPRITE_2D_VERT_COUNT_MAX = SPRITE_2D_COUNT_MAX * QUAD_VERTEX_COUNT;
        constexpr int SPRITE_2D_IDX_COUNT_MAX  = SPRITE_2D_COUNT_MAX * QUAD_IDX_COUNT;
        constexpr int SHAPE_2D_VERT_COUNT_MAX  = (SHAPE_2D_COUNT_MAX * 2) * TRI_VERTEX_COUNT;
        constexpr int SHAPE_2D_IDX_COUNT_MAX   = SHAPE_2D_VERT_COUNT_MAX;
        constexpr int GLYPH_2D_VERT_COUNT_MAX  = GLYPH_2D_COUNT_MAX * QUAD_VERTEX_COUNT;
        constexpr int GLYPH_2D_IDX_COUNT_MAX   = GLYPH_2D_COUNT_MAX * QUAD_IDX_COUNT;
        constexpr int TRI_3D_VERT_COUNT_MAX    = TRI_3D_COUNT_MAX * TRI_VERTEX_COUNT;
        constexpr int TRI_3D_IDX_COUNT_MAX     = TRI_3D_COUNT_MAX * TRI_IDX_COUNT;

        constexpr int VERT_2D_COUNT_MAX       = SPRITE_2D_VERT_COUNT_MAX +
                                                SHAPE_2D_VERT_COUNT_MAX +
                                                GLYPH_2D_VERT_COUNT_MAX;
        constexpr int VERT_2D_IDX_COUNT_MAX   = SPRITE_2D_IDX_COUNT_MAX +
                                                SHAPE_2D_IDX_COUNT_MAX +
                                                GLYPH_2D_IDX_COUNT_MAX;
        constexpr int PRIM_2D_BATCH_COUNT_MAX = SPRITE_2D_COUNT_MAX +
                                                SHAPE_2D_COUNT_MAX +
                                                GLYPH_2D_COUNT_MAX;
        constexpr int VERT_3D_COUNT_MAX       = TRI_3D_VERT_COUNT_MAX;
        constexpr int VERT_3D_IDX_COUNT_MAX   = VERT_3D_COUNT_MAX;
        constexpr int PRIM_3D_BATCH_COUNT_MAX = (VERT_3D_IDX_COUNT_MAX / TRI_VERTEX_COUNT) * 2; // @todo Check, because immediate + models.

        // Initialize GPU buffers.
        _gpuBuffers.ViewportVertices.Initialize(*_device, QUAD_VERTEX_COUNT, QUAD_IDX_COUNT, "2D viewport vertices");
        _gpuBuffers.ImmediateVertices2d.Initialize(*_device, VERT_2D_COUNT_MAX, VERT_2D_IDX_COUNT_MAX, "Immediate 2D vertices");
        _gpuBuffers.ImmediateVertices3d.Initialize(*_device, VERT_3D_COUNT_MAX, VERT_3D_IDX_COUNT_MAX, "Immediate 3D vertices");
        _meshes = std::make_unique<MeshCache>(*_device, VERT_3D_COUNT_MAX, VERT_3D_IDX_COUNT_MAX, "3D meshes vertices");

        // Reserve draw batches.
        _drawBatches.Primitives2d.reserve(PRIM_2D_BATCH_COUNT_MAX);
        _drawBatches.Primitives3d.reserve(PRIM_3D_BATCH_COUNT_MAX);
    }

    void Renderer::UpdateResources(SDL_GPUCopyPass& copyPass)
    {
        constexpr auto FONT_ATLAS_RES = Vector2i(Font::ATLAS_SIZE);

        auto& fonts = g_App.GetFonts();
        auto& video = g_App.GetVideo();

        // Release/upload textures.
        auto& texs = GetTextures();
        for (const auto& assetName : _doubleBuffer.Render.TextureReleaseQueue)
        {
            texs.Release(assetName);
        }
        for (const auto& assetName : _doubleBuffer.Render.TextureUploadQueue)
        {
            texs.Upload(copyPass, assetName);
        }

        // Release/upload meshes.
        auto& meshes = GetMeshes();
        for (const auto& assetName : _doubleBuffer.Render.MeshReleaseQueue)
        {
            meshes.Release(assetName);
        }
        for (const auto& assetName : _doubleBuffer.Render.MeshUploadQueue)
        {
            meshes.Upload(copyPass, assetName);
        }

        // Run through registered fonts.
        for (const auto& metadata : FONTS_METADATA)
        {
            auto* font = fonts.GetFont(metadata.Name);
            if (font == nullptr)
            {
                continue;
            }

            const auto& atlases = font->GetTextureAtlases();
            for (int atlasIdx : font->GetDirtyGpuAtlasIdxs())
            {
                const auto& atlas = atlases[atlasIdx];

                // Upload/update font atlas texture.
                auto  name = metadata.Name + std::to_string(atlasIdx);
                auto* tex  = GetTextures()[name];
                if (tex != nullptr)
                {
                    tex->Update(copyPass, ToSpan(atlas), Vector2i::Zero, FONT_ATLAS_RES);
                }
                else
                {
                    GetTextures().Upload(copyPass, ToSpan(atlas), FONT_ATLAS_RES, name);
                }
            }

            font->ClearDirtyGpuAtlasIdxs();
        }

        // Upload/update video texture.
        const auto& videoName = video.GetName();
        if (video.IsPlaying())
        {
            auto* tex = GetTextures()[videoName];
            if (tex != nullptr)
            {
                tex->Update(copyPass, ToSpan(video.GetVideoFrame()), Vector2i::Zero, video.GetResolution());
            }
            else
            {
                GetTextures().Upload(copyPass, ToSpan(video.GetVideoFrame()), video.GetResolution(), video.GetName());
            }
        }
        // Release video texture.
        else if (video.IsLoaded())
        {
            GetTextures().Release(videoName);
        }
    }

    void Renderer::CopyImmediatePrimitives2d(SDL_GPUCopyPass& copyPass)
    {
        auto bufferVerts = std::vector<BufferVertex2d>{};
        auto bufferIdxs  = std::vector<uint16>{};

        // Reserve memory.
        bufferVerts.reserve(_doubleBuffer.Render.ImmediatePrimitives2d.size() * QUAD_VERTEX_COUNT);
        bufferIdxs.reserve(_doubleBuffer.Render.ImmediatePrimitives2d.size() * QUAD_IDX_COUNT);

        // Create batched GPU buffer data.
        int vertOffset = 0;
        int idxOffset  = 0;
        for (const auto& prim : _doubleBuffer.Render.ImmediatePrimitives2d)
        {
            // Add vertices.
            for (int i = 0; i < prim.Vertices.size(); i++)
            {
                float depthZ = std::clamp((float)prim.Depth / (float)DEPTH_MAX, 0.0f, 1.0f);
                auto  pos    = Vector3(prim.Vertices[i].Position.x, prim.Vertices[i].Position.y, depthZ);
                bufferVerts.push_back(BufferVertex2d
                {
                    .Position = pos,
                    .Uv       = prim.Vertices[i].Uv,
                    .Col      = prim.Vertices[i].Col
                });
            }

            int curVertCount = 0;
            int curIdxCount  = 0;

            // Triangle.
            if (prim.Vertices.size() == TRI_VERTEX_COUNT)
            {
                // Add indices.
                for (int i = 0; i < TRI_IDX_COUNT; i++)
                {
                    bufferIdxs.push_back(i);
                }

                // Set buffer region.
                curVertCount = TRI_VERTEX_COUNT;
                curIdxCount  = TRI_IDX_COUNT;
            }
            // Quad.
            else if (prim.Vertices.size() == QUAD_VERTEX_COUNT)
            {
                // Add indices.
                for (int i : QUAD_TRI_IDXS)
                {
                    bufferIdxs.push_back(i);
                }

                // Set buffer region.
                curVertCount = QUAD_VERTEX_COUNT;
                curIdxCount  = QUAD_IDX_COUNT;
            }

            // Add batch.
            // @todo Smarter way that strings together primitives with the same render stage, blend mode, and texture.
            // What to do with uniforms? For now, collect each as its own batch of 2 triangles.
            _drawBatches.Primitives2d.push_back(DrawBatch
            {
                .TextureName  = prim.TextureName,
                .RenderStg    = prim.RenderStg,
                .BlendMd      = prim.BlendMd,
                .Uniform      = prim.Uniform,
                .VertexCount  = curIdxCount,
                .VertexOffset = vertOffset,
                .IdxOffset    = idxOffset
            });

            vertOffset += curVertCount;
            idxOffset  += curIdxCount;
        }

        // Update GPU buffer.
        if (!bufferVerts.empty() && !bufferIdxs.empty())
        {
            _gpuBuffers.ImmediateVertices2d.UpdateVertices(copyPass, ToSpan(bufferVerts), 0);
            _gpuBuffers.ImmediateVertices2d.UpdateIdxs(copyPass, ToSpan(bufferIdxs), 0);
        }
    }

    void Renderer::CopyImmediatePrimitives3d(SDL_GPUCopyPass& copyPass)
    {
        auto bufferVerts = std::vector<BufferVertex3d>{};
        auto bufferIdxs  = std::vector<uint16>{};

        // Reserve memory.
        bufferVerts.reserve(_doubleBuffer.Render.ImmediatePrimitives3d.size() * TRI_VERTEX_COUNT);
        bufferIdxs.reserve(_doubleBuffer.Render.ImmediatePrimitives3d.size() * TRI_IDX_COUNT);

        // Create batched GPU buffer data.
        int vertOffset = 0;
        int idxOffset  = 0;
        for (const auto& prim : _doubleBuffer.Render.ImmediatePrimitives3d)
        {
            // Add vertices.
            for (int i = 0; i < prim.Vertices.size(); i++)
            {
                bufferVerts.push_back(BufferVertex3d
                {
                    .Position = prim.Vertices[i].Position,
                    .Normal   = prim.Vertices[i].Normal,
                    .Uv       = prim.Vertices[i].Uv,
                    .Col      = prim.Vertices[i].Col
                });
            }

            int curVertCount = 0;
            int curIdxCount  = 0;

            // @todo
            // Line.
            /*if (prim.Vertices.size() == LINE_VERTEX_COUNT)
            {
                // Add indices.
                for (int i = 0; i < LINE_IDX_COUNT; i++)
                {
                    bufferIdxs.push_back(i);
                }

                // Set buffer region.
                curVertCount = LINE_VERTEX_COUNT;
                curIdxCount  = LINE_IDX_COUNT;
            }
            else */if (prim.Vertices.size() == TRI_VERTEX_COUNT)
            {
                // Add indices.
                for (int i = 0; i < TRI_IDX_COUNT; i++)
                {
                    bufferIdxs.push_back(i);
                }

                // Set buffer region.
                curVertCount = TRI_VERTEX_COUNT;
                curIdxCount  = TRI_IDX_COUNT;
            }
            // Quad.
            else if (prim.Vertices.size() == QUAD_VERTEX_COUNT)
            {
                // Add indices.
                for (int i : QUAD_TRI_IDXS)
                {
                    bufferIdxs.push_back(i);
                }

                // Set buffer region.
                curVertCount = QUAD_VERTEX_COUNT;
                curIdxCount  = QUAD_IDX_COUNT;
            }

            // Add batch.
            // @todo Smarter way that strings together primitives with the same render stage, blend mode, and texture.
            // What to do with uniforms? For now, collect each as its own batch of 2 triangles.
            _drawBatches.Primitives3d.push_back(DrawBatch
            {
                .TextureName  = prim.TextureName,
                .RenderStg    = prim.RenderStg,
                .BlendMd      = prim.BlendMd,
                .Uniform      = prim.Uniform,
                .VertexCount  = curIdxCount,
                .VertexOffset = vertOffset,
                .IdxOffset    = idxOffset
            });

            vertOffset += curVertCount;
            idxOffset  += curIdxCount;
        }

        // Update GPU buffer.
        if (!bufferVerts.empty() && !bufferIdxs.empty())
        {
            _gpuBuffers.ImmediateVertices3d.UpdateVertices(copyPass, ToSpan(bufferVerts), 0);
            _gpuBuffers.ImmediateVertices3d.UpdateIdxs(copyPass, ToSpan(bufferIdxs), 0);
        }
    }

    void Renderer::CopyGpuViewportQuad(SDL_GPUCopyPass& copyPass)
    {
        constexpr auto BUFFER_IDXS = std::array<uint16, QUAD_IDX_COUNT>{ 0, 2, 1, 1, 2, 3 };

        // @todo Compute aspect-correct vertex positions.
        auto bufferVerts = std::vector<BufferVertex2d>
        {
            {
                .Position = Vector3(-1.0f, 1.0f, 0.0f),
                .Uv       = Vector2(0.0f, 0.0f),
                .Col      = Color::White
            },
            {
                .Position = Vector3(1.0f, 1.0f, 0.0f),
                .Uv       = Vector2(1.0f, 0.0f),
                .Col      = Color::White
            },
            {
                .Position = Vector3(-1.0f, -1.0f, 0.0f),
                .Uv       = Vector2(0.0f, 1.0f),
                .Col      = Color::White
            },
            {
                .Position = Vector3(1.0f, -1.0f, 0.0f),
                .Uv       = Vector2(1.0f, 1.0f),
                .Col      = Color::White
            }
        };

        // Update GPU buffer.
        _gpuBuffers.ViewportVertices.UpdateVertices(copyPass, ToSpan(bufferVerts), 0);
        _gpuBuffers.ViewportVertices.UpdateIdxs(copyPass, ToSpan(BUFFER_IDXS), 0);
    }

    void Renderer::PushVertexUniform(const UniformType& uni, int slotIdx)
    {
        std::visit([&](auto&& arg)
        {
            SDL_PushGPUVertexUniformData(_commandBuffer, slotIdx, &arg, sizeof(arg));
        }, uni);
    }

    void Renderer::PushFragmentUniform(const UniformType& uni, int slotIdx)
    {
        std::visit([&](auto&& arg)
        {
            SDL_PushGPUFragmentUniformData(_commandBuffer, slotIdx, &arg, sizeof(arg));
        }, uni);
    }

    void Renderer::ClearDrawBatches()
    {
        _drawBatches.Primitives2d.clear();
        _drawBatches.Primitives3d.clear();
    }
}
