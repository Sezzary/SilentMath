#include "Framework.h"
#include "Renderer/Backends/SdlGpu/SdlGpu.h"

#include "Application.h"
#include "Assets/Fonts.h"
#include "Renderer/Backends/SdlGpu/Pipeline/Pipeline.h"
#include "Renderer/Backends/SdlGpu/Resources/Texture.h"
#include "Renderer/Common/Resources/Buffers.h"
#include "Renderer/Common/Resources/Uniforms.h"
#include "Renderer/Common/Texture.h"
#include "Renderer/Common/Utils.h"
#include "Renderer/Common/View.h" // @todo Not used yet.
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

        // Initialize buffers.
        InitializeDoubleBuffer();
        InitializeGpuBuffers();

        // Initialize texture manager.
        _textures = std::make_unique<TextureManager>(*_device);

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

        // @todo If this isn't called and the texture is missing, for some reason
        // the app hangs instead of crashing like it's supposed to. Why isn't such an error
        // handled as written?
        UpdateFontAtlasTextures(*copyPass);

        // Load temp. textures.
        GetTextures().Load(*copyPass, "TIM/HERO_PIC.TIM");
        GetTextures().Load(*copyPass, "1ST/2ZANKO_E.TIM");
        GetTextures().Load(*copyPass, "TIM/BG_ETC.TIM");

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

    void Renderer::Update()
    {
        // Frame setup.
        SortRenderBufferData();
        ClearDrawBatches();
    
        // Acquire command buffer.
        _commandBuffer = SDL_AcquireGPUCommandBuffer(_device);
        if (_commandBuffer == nullptr)
        {
            Debug::Log(Fmt("Failed to acquire command buffer: {}", SDL_GetError()), Debug::LogLevel::Error);
            return;
        }

        // Acquire render texture.
        _renderTexture = GetRenderTexture();
        if (_renderTexture == nullptr)
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

        UpdateFontAtlasTextures(*copyPass);

        SDL_EndGPUCopyPass(copyPass);

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
        auto res = GetScreenResolution();

        // Ensure directory exists.
        auto timestamp = GetCurrentDateString() + "_" + GetCurrentTimeString();
        auto filename  = (SCREENSHOT_FILENAME_BASE + timestamp) + PNG_FILE_EXT;
        auto path      = fs.GetScreenshotsDirectory() / filename;
        std::filesystem::create_directories(path.parent_path());

        // Get window surface.
        auto* surface = SDL_GetWindowSurface(_window);
        if (surface == nullptr)
        {
            Debug::Log(Fmt("Failed to save screenshot: {}", SDL_GetError()), Debug::LogLevel::Warning, Debug::LogMode::All, true);
            return;
        }

        // Lock surface to access pixels.
        if (!SDL_LockSurface(surface))
        {
            Debug::Log(Fmt("Failed to save screenshot: {}", SDL_GetError()), Debug::LogLevel::Warning, Debug::LogMode::All, true);
            return;
        }

        // Write screenshot file.
        if (stbi_write_png(path.string().c_str(), res.x, res.y, COLOR_CHANNEL_COUNT, surface->pixels, res.x * COLOR_CHANNEL_COUNT))
        {
            Debug::Log("Saved screenshot.", Debug::LogLevel::Info, Debug::LogMode::All, true);
        }
        else
        {
            Debug::Log("Failed to save screenshot.", Debug::LogLevel::Warning, Debug::LogMode::All, true);
        }

        SDL_UnlockSurface(surface);
    }

    SDL_GPUTexture* Renderer::GetRenderTexture()
    {
        // @todo Entering fullscreen mode doesn't signal a resize?
        if (_renderTexture == nullptr || _isResized)
        {
            SDL_ReleaseGPUTexture(_device, _renderTexture);

            // @todo Size should depend on aspect ratio: 4:3, 16:9, or native.

            auto screenRes     = GetScreenResolution();
            auto renderTexInfo = SDL_GPUTextureCreateInfo
            {
                .type                 = SDL_GPU_TEXTURETYPE_2D,
                .format               = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
                .usage                = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER,
                .width                = screenRes.x,
                .height               = screenRes.y,
                .layer_count_or_depth = 1,
                .num_levels           = 1
            };
            _renderTexture = SDL_CreateGPUTexture(_device, &renderTexInfo);
        }

        return _renderTexture;
    }

    TextureManager& Renderer::GetTextures()
    {
        return *(TextureManager*)_textures.get();
    }

    SDL_GPUSampler& Renderer::GetActiveSampler()
    {
        const auto& options = g_App.GetOptions();

        return *_samplers[(int)options->TextureFilter];
    }

    void Renderer::Draw3dScene()
    {
        // Begin render pass.
        auto colorTargetInfo = SDL_GPUColorTargetInfo
        {
            .texture     = _renderTexture,
            .clear_color = SDL_FColor{ _clearColor.R(), _clearColor.G(), _clearColor.B(), _clearColor.A() },
            .load_op     = SDL_GPU_LOADOP_CLEAR,
            .store_op    = SDL_GPU_STOREOP_STORE
        };
        auto& renderPass = *SDL_BeginGPURenderPass(_commandBuffer, &colorTargetInfo, 1, nullptr);

        // @todo

        // Process render pass.
        SDL_EndGPURenderPass(&renderPass);
    }

    void Renderer::DrawDither()
    {
        auto& executor = g_App.GetExecutor();
        auto& options  = g_App.GetOptions();
        
        // Process copy pass.
        auto* copyPass = SDL_BeginGPUCopyPass(_commandBuffer);

        // Copy prepared GPU data.
        auto copyTasks = ParallelTasks
        {
            TASK(CopyGpuViewportQuad(*copyPass))
        };
        executor.AddTasks(copyTasks).wait();

        SDL_EndGPUCopyPass(copyPass);

        // Begin render pass.
        auto colorTargetInfo = SDL_GPUColorTargetInfo
        {
            .texture  = _renderTexture,
            .load_op  = SDL_GPU_LOADOP_LOAD,
            .store_op = SDL_GPU_STOREOP_STORE
        };
        auto& renderPass = *SDL_BeginGPURenderPass(_commandBuffer, &colorTargetInfo, 1, nullptr);

        // @todo Severe visual artefacts.
        // Dither.
        if (options->EnableDithering)
        {
            _pipelines.Bind(renderPass, RenderStage::Dither, BlendMode::Opaque);

            // Bind render texture.
            auto binding = SDL_GPUTextureSamplerBinding
            {
                .texture = _renderTexture,
                .sampler = _samplers[(int)TextureFilterType::Nearest]
            };
            SDL_BindGPUFragmentSamplers(&renderPass, 0, &binding, 1);

            SDL_DrawGPUIndexedPrimitives(&renderPass, QUAD_IDX_COUNT, 1, 0, 0, 0);
            _doubleBuffer.Active.DrawCallCount++;
        }

        // End render pass.
        SDL_EndGPURenderPass(&renderPass);
    }

    void Renderer::Draw2dScene()
    {
        auto& executor = g_App.GetExecutor();

        // Process copy pass.
        auto* copyPass = SDL_BeginGPUCopyPass(_commandBuffer);

        // Copy prepared GPU data.
        auto copyTasks = ParallelTasks
        {
            TASK(CopyGpuPrimitives2d(*copyPass))
        };
        executor.AddTasks(copyTasks).wait();

        SDL_EndGPUCopyPass(copyPass);

        // Begin render pass.
        auto colorTargetInfo = SDL_GPUColorTargetInfo
        {
            .texture  = _renderTexture,
            .load_op  = SDL_GPU_LOADOP_LOAD,
            .store_op = SDL_GPU_STOREOP_STORE
        };
        auto& renderPass = *SDL_BeginGPURenderPass(_commandBuffer, &colorTargetInfo, 1, nullptr);

        // 2D primitives.
        _gpuBuffers.Vertices2d.Bind(renderPass, 0, 0);
        for (const auto& batch : _drawBatches.Primitives2d)
        {
            _pipelines.Bind(renderPass, batch.RenderStg, batch.BlendMd);
            PushFragmentUniform(batch.Uniform, 0);

            if (!batch.TextureName.empty())
            {
                auto* tex = GetTextures()[batch.TextureName];
                if (tex != nullptr)
                {
                    tex->Bind(renderPass, GetActiveSampler());
                }
            }

            SDL_DrawGPUIndexedPrimitives(&renderPass, batch.BufferStride, 1, 0, batch.BufferOffset, 0);
            _doubleBuffer.Active.DrawCallCount++;
        }

        // End render pass.
        SDL_EndGPURenderPass(&renderPass);
    }

    void Renderer::DrawPostProcess()
    {
        auto&       executor = g_App.GetExecutor();
        const auto& options  = g_App.GetOptions();

        // Process copy pass.
        auto* copyPass = SDL_BeginGPUCopyPass(_commandBuffer);

        // Copy prepared GPU data.
        auto copyTasks = ParallelTasks
        {
            TASK(CopyGpuViewportQuad(*copyPass))
        };
        executor.AddTasks(copyTasks).wait();

        SDL_EndGPUCopyPass(copyPass);

        // Begin render pass.
        auto colorTargetInfo = SDL_GPUColorTargetInfo
        {
            .texture  = _renderTexture,
            .load_op  = SDL_GPU_LOADOP_LOAD,
            .store_op = SDL_GPU_STOREOP_STORE
        };
        auto& renderPass = *SDL_BeginGPURenderPass(_commandBuffer, &colorTargetInfo, 1, nullptr);

        // Process render pass.
        _gpuBuffers.ViewportVertices2d.Bind(renderPass, 0, 0);

        // @todo Severe visual artefacts.
        // Temp. test dither.
        /*if (options->EnableDithering)
        {
            _pipelines.Bind(renderPass, RenderStage::Dither, BlendMode::Opaque);

            // Bind render texture.
            auto binding = SDL_GPUTextureSamplerBinding
            {
                .texture = _renderTexture,
                .sampler = _samplers[(int)TextureFilterType::Nearest]
            };
            SDL_BindGPUFragmentSamplers(&renderPass, 0, &binding, 1);

            SDL_DrawGPUIndexedPrimitives(&renderPass, QUAD_IDX_COUNT, 1, 0, 0, 0);
            _doubleBuffer.Active.DrawCallCount++;
        }*/

        // Luma-based fade.
        {
            _pipelines.Bind(renderPass, RenderStage::Fade, BlendMode::Opaque);
            PushFragmentUniform(UniformLumaFade{ .FadeAlpha = Debug::g_Work.BlendAlpha }, 0);

            // Bind render texture.
            auto binding = SDL_GPUTextureSamplerBinding
            {
                .texture = _renderTexture,
                .sampler = _samplers[(int)TextureFilterType::Nearest]
            };
            SDL_BindGPUFragmentSamplers(&renderPass, 0, &binding, 1);

            SDL_DrawGPUIndexedPrimitives(&renderPass, QUAD_IDX_COUNT, 1, 0, 0, 0);
            _doubleBuffer.Active.DrawCallCount++;
        }

        // @todo Severe visual artefacts.
        // CRT filter.
        if (options->EnableCrtFilter)
        {
            _pipelines.Bind(renderPass, RenderStage::Crt, BlendMode::Opaque);
            PushFragmentUniform(UniformCrt{ .Resolution = GetScreenResolution().ToVector2(), .Time = 0.0f }, 0);

            // Bind render texture.
            auto binding = SDL_GPUTextureSamplerBinding
            {
                .texture = _renderTexture,
                .sampler = _samplers[(int)TextureFilterType::Nearest]
            };
            SDL_BindGPUFragmentSamplers(&renderPass, 0, &binding, 1);

            SDL_DrawGPUIndexedPrimitives(&renderPass, QUAD_IDX_COUNT, 1, 0, 0, 0);
            _doubleBuffer.Active.DrawCallCount++;
        }

        // @todo Severe visual artefacts.
        // Vignette.
        if (options->EnableVignette)
        {
            _pipelines.Bind(renderPass, RenderStage::Vignette, BlendMode::Opaque);
            PushFragmentUniform(UniformCrt{ .Resolution = GetScreenResolution().ToVector2(), .Time = 0.0f }, 0);

            // Bind render texture.
            auto binding = SDL_GPUTextureSamplerBinding
            {
                .texture = _renderTexture,
                .sampler = _samplers[(int)TextureFilterType::Nearest]
            };
            SDL_BindGPUFragmentSamplers(&renderPass, 0, &binding, 1);

            SDL_DrawGPUIndexedPrimitives(&renderPass, QUAD_IDX_COUNT, 1, 0, 0, 0);
            _doubleBuffer.Active.DrawCallCount++;
        }

        SDL_EndGPURenderPass(&renderPass);
    }

    void Renderer::DrawViewport()
    {
        constexpr float BRIGHTNESS_STEP   = 0.25f / BRIGHTNESS_LEVEL_MAX;
        constexpr float BRIGHTNESS_MIDDLE = BRIGHTNESS_STEP * (BRIGHTNESS_LEVEL_MAX / 2);

        const auto& options = g_App.GetOptions();

        // Begin render pass.
        auto colorTargetInfo = SDL_GPUColorTargetInfo
        {
            .texture     = _swapchainTexture,
            .clear_color = SDL_FColor{ Color::Black.R(), Color::Black.G(), Color::Black.B(), Color::Black.A() },
            .load_op     = SDL_GPU_LOADOP_CLEAR,
            .store_op    = SDL_GPU_STOREOP_STORE
        };
        auto& renderPass = *SDL_BeginGPURenderPass(_commandBuffer, &colorTargetInfo, 1, nullptr);

        // Process render pass.

        _pipelines.Bind(renderPass, RenderStage::Blit, BlendMode::Opaque);
        _gpuBuffers.ViewportVertices2d.Bind(renderPass, 0, 0);

        // Push uniform.
        float brightness = (BRIGHTNESS_STEP * options->BrightnessLevel) - BRIGHTNESS_MIDDLE;
        auto  uni        = UniformBlit
        {
            .Brightness = brightness
        };
        PushFragmentUniform(uni, 0);

        // Bind render texture.
        auto binding = SDL_GPUTextureSamplerBinding
        {
            .texture = _renderTexture,
            .sampler = _samplers[(int)TextureFilterType::Nearest]
        };
        SDL_BindGPUFragmentSamplers(&renderPass, 0, &binding, 1);

        // Draw screen quad.
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

        // Begin render pass.
        auto colorTargetInfo = SDL_GPUColorTargetInfo
        {
            .texture  = _swapchainTexture,
            .load_op  = SDL_GPU_LOADOP_LOAD,
            .store_op = SDL_GPU_STOREOP_STORE
        };
        auto* renderPass = SDL_BeginGPURenderPass(_commandBuffer, &colorTargetInfo, 1, nullptr);

        // Process render pass.
        ImGui_ImplSDLGPU3_RenderDrawData(drawData, _commandBuffer, renderPass);

        SDL_EndGPURenderPass(renderPass);
    }

    void Renderer::InitializeGpuBuffers()
    {
        constexpr int SPRITE_2D_VERT_COUNT_MAX = SPRITE_2D_COUNT_MAX * QUAD_VERTEX_COUNT;
        constexpr int SPRITE_2D_IDX_COUNT_MAX  = SPRITE_2D_COUNT_MAX * QUAD_IDX_COUNT;
        constexpr int SHAPE_2D_VERT_COUNT_MAX  = (SHAPE_2D_COUNT_MAX * 2) * TRI_VERTEX_COUNT;
        constexpr int SHAPE_2D_IDX_COUNT_MAX   = SHAPE_2D_VERT_COUNT_MAX;
        constexpr int TRI_BATCH_COUNT_MAX      = SPRITE_2D_COUNT_MAX +
                                                 SHAPE_2D_COUNT_MAX;
        constexpr int TRI_VERT_COUNT_MAX       = SPRITE_2D_VERT_COUNT_MAX +
                                                 SHAPE_2D_VERT_COUNT_MAX;
        constexpr int TRI_IDX_COUNT_MAX        = SPRITE_2D_IDX_COUNT_MAX;

        // Reserve draw batches.
        _drawBatches.Primitives2d.reserve(TRI_BATCH_COUNT_MAX);

        // Initialize GPU buffers.
        _gpuBuffers.ViewportVertices2d.Initialize(*_device, QUAD_VERTEX_COUNT, QUAD_IDX_COUNT, "2D screen vertices");
        _gpuBuffers.Vertices2d.Initialize(*_device, TRI_VERT_COUNT_MAX, TRI_IDX_COUNT_MAX, "2D vertices");
    }

    void Renderer::UpdateFontAtlasTextures(SDL_GPUCopyPass& copyPass)
    {
        auto& fonts = g_App.GetFonts();

        // Run through registered fonts.
        for (const auto& metadata : FONTS_METADATA)
        {
            auto* font = fonts.GetFont(metadata.Name);
            if (font != nullptr)
            {
                const auto& atlases = font->GetTextureAtlases();
                for (int atlasIdx : font->GetDirtyGpuAtlasIdxs())
                {
                    const auto& atlas = atlases[atlasIdx];

                    // Initialize new or update existing GPU font atlas textures.
                    // @todo Not efficient. Updates all atlases even if only 1 has been updated.
                    auto  name = metadata.Name + std::to_string(atlasIdx);
                    auto* tex  = GetTextures()[name];
                    if (tex != nullptr)
                    {
                        tex->Update(copyPass, ToSpan(atlas), Vector2i::Zero, Vector2i(Font::ATLAS_SIZE));
                    }
                    else
                    {
                        GetTextures().Load(copyPass, ToSpan(atlas), Vector2i(Font::ATLAS_SIZE), name);
                    }
                }

                font->ClearDirtyGpuAtlasIdxs();
            }
        }
    }

    void Renderer::CopyGpuPrimitives2d(SDL_GPUCopyPass& copyPass)
    {
        auto bufferVerts = std::vector<BufferVertex2d>{};
        auto bufferIdxs  = std::vector<uint16>{};

        // Reserve memory.
        bufferVerts.reserve(_doubleBuffer.Render.Primitives2d.size() * QUAD_VERTEX_COUNT);
        bufferIdxs.reserve(_doubleBuffer.Render.Primitives2d.size() * QUAD_IDX_COUNT);

        // Create batched GPU buffer data.
        int vertOffset = 0;
        for (const auto& prim : _doubleBuffer.Render.Primitives2d)
        {
            // Add vertices.
            for (int i = 0; i < prim.Vertices.size(); i++)
            {
                // @todo Z depth oesn't seem to have any effect and primitives still need manual depth sorting.
                float depthZ = std::clamp((float)prim.Depth / (float)DEPTH_MAX, 0.0f, 1.0f);
                auto  pos    = Vector3(prim.Vertices[i].Position.x, prim.Vertices[i].Position.y, depthZ);
                bufferVerts.push_back(BufferVertex2d{ pos, prim.Vertices[i].Uv, prim.Vertices[i].Col });
            }
    
            int curVertCount = 0;
            int curIdxCount  = 0;

            // Triangle.
            if (prim.Vertices.size() == TRI_VERTEX_COUNT)
            {
                // Add indices.
                for (int i = 0; i < TRI_IDX_COUNT; i++)
                {
                    bufferIdxs.push_back(vertOffset + i);
                }

                curVertCount = TRI_VERTEX_COUNT;
                curIdxCount  = TRI_IDX_COUNT;
            }
            // Quad.
            else if (prim.Vertices.size() == QUAD_VERTEX_COUNT)
            {
                // Add indices.
                for (int quadTriIdx : QUAD_TRI_IDXS)
                {
                    bufferIdxs.push_back(vertOffset + quadTriIdx);
                }

                curVertCount = QUAD_VERTEX_COUNT;
                curIdxCount  = QUAD_IDX_COUNT;
            }
    
            // Add batch.
            // @todo Smarter way that strings together primitives with the same render stage, blend mode, and texture. What to do with uniforms?
            // For now, collect each as its own batch of 2 triangles.
            _drawBatches.Primitives2d.push_back(DrawBatch
            {
                .TextureName  = prim.TextureName,
                .RenderStg    = prim.RenderStg,
                .BlendMd      = prim.BlendMd,
                .Uniform      = prim.Uniform,
                .BufferOffset = vertOffset,
                .BufferStride = curIdxCount
            });

            vertOffset += curVertCount;
        }

        // Update GPU buffer.
        _gpuBuffers.Vertices2d.UpdateVertices(copyPass, ToSpan(bufferVerts), 0);
        _gpuBuffers.Vertices2d.UpdateIdxs(copyPass, ToSpan(bufferIdxs), 0);
    }

    void Renderer::CopyGpuViewportQuad(SDL_GPUCopyPass& copyPass)
    {
        constexpr auto BUFFER_IDXS = std::array<uint16, QUAD_IDX_COUNT>{ 0, 2, 1, 1, 2, 3 };

        // @todo Compute aspect-correct vertex positions.
        auto bufferVerts = std::vector<BufferVertex2d>
        {
            BufferVertex2d
            {
                Vector3(-1.0f, 1.0f, 0.0f),
                Vector2(0.0f, 0.0f),
                Color::White
            },
            BufferVertex2d
            {
                Vector3(1.0f, 1.0f, 0.0f),
                Vector2(1.0f, 0.0f),
                Color::White
            },
            BufferVertex2d
            {
                Vector3(-1.0f, -1.0f, 0.0f),
                Vector2(0.0f, 1.0f),
                Color::White
            },
            BufferVertex2d
            {
                Vector3(1.0f, -1.0f, 0.0f),
                Vector2(1.0f, 1.0f),
                Color::White
            }
        };

        // Update GPU buffer.
        _gpuBuffers.ViewportVertices2d.UpdateVertices(copyPass, ToSpan(bufferVerts), 0);
        _gpuBuffers.ViewportVertices2d.UpdateIdxs(copyPass, ToSpan(BUFFER_IDXS), 0);
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
    }
}
