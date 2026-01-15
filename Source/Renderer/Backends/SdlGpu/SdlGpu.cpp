#include "Framework.h"
#include "Renderer/Backends/SdlGpu/SdlGpu.h"

#include "Application.h"
#include "Renderer/Backends/SdlGpu/Gpu/Layouts/Triangle2dUniform.h"
#include "Renderer/Backends/SdlGpu/Gpu/Layouts/Vertex2dBuffer.h"
#include "Renderer/Backends/SdlGpu/Pipeline.h"
#include "Renderer/Backends/SdlGpu/Texture.h"
#include "Renderer/Common/Texture.h"
#include "Renderer/Common/Utils.h"
#include "Renderer/Common/View.h" // @todo Not used yet.
#include "Renderer/Renderer.h"
#include "Services/Filesystem.h"
#include "Services/Options.h"
#include "Services/Platform.h"
#include "Utils/Utils.h"

using namespace Silent::Services;
using namespace Silent::Utils;

namespace Silent::Renderer
{
    void SdlGpuRenderer::Initialize(SDL_Window& window)
    {
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

        InitializeDoubleBuffer();

        // Initialize texture manager.
        _textures = std::make_unique<SdlGpuTextureManager>(*_device);

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

        AllocateMemory();

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

        // Load temp. textures.
        GetTextures().Load(*copyPass, "TIM/HERO_PIC.TIM");
        GetTextures().Load(*copyPass, "1ST/2ZANKO_E.TIM");

        SDL_EndGPUCopyPass(copyPass);
        SDL_SubmitGPUCommandBuffer(uploadCmdBuffer);
    }

    void SdlGpuRenderer::Deinitialize()
    {
        // @todo Errors.

        SDL_WaitForGPUIdle(_device);

        ImGui_ImplSDL3_Shutdown();
        ImGui_ImplSDLGPU3_Shutdown();
        ImGui::DestroyContext();

        //_textures TestTexture.~SdlGpuTexture();
        _gpuBuffers = {};
        _pipelines.Deinitialize();

        SDL_ReleaseWindowFromGPUDevice(_device, _window);
        SDL_DestroyGPUDevice(_device);
    }

    void SdlGpuRenderer::Update()
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

        // Acquire swapchain texture.
        _swapchainTexture = nullptr;
        if (!SDL_WaitAndAcquireGPUSwapchainTexture(_commandBuffer, _window, &_swapchainTexture, nullptr, nullptr))
        {
            return;
        }

        // Draw frame.
        if (_swapchainTexture != nullptr)
        {
            Draw3dScene();
            Draw2dScene();
            DrawPostProcess();
            DrawDebugGui();
        }

        // Submit command buffer to GPU.
        SDL_SubmitGPUCommandBuffer(_commandBuffer);
    }

    void SdlGpuRenderer::SaveScreenshot() const
    {
        constexpr int COLOR_CHANNEL_COUNT = 3; // RGB.

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

    void SdlGpuRenderer::Draw3dScene()
    {
        // Begin render pass.
        auto colorTargetInfo = SDL_GPUColorTargetInfo
        {
            .texture     = _swapchainTexture,
            .clear_color = SDL_FColor{ _clearColor.R(), _clearColor.G(), _clearColor.B(), _clearColor.A() },
            .load_op     = SDL_GPU_LOADOP_CLEAR,
            .store_op    = SDL_GPU_STOREOP_STORE
        };
        auto& renderPass = *SDL_BeginGPURenderPass(_commandBuffer, &colorTargetInfo, 1, nullptr);

        // @todo

        // Process render pass.
        SDL_EndGPURenderPass(&renderPass);
    }

    SdlGpuTextureManager& SdlGpuRenderer::GetTextures()
    {
        return *(SdlGpuTextureManager*)_textures.get();
    }

    SDL_GPUSampler& SdlGpuRenderer::GetActiveSampler()
    {
        const auto& options = g_App.GetOptions();

        return *_samplers[(int)options->TextureFilter];
    }

    void SdlGpuRenderer::Draw2dScene()
    {
        auto& executor = g_App.GetExecutor();

        // Process copy pass.
        auto* copyPass = SDL_BeginGPUCopyPass(_commandBuffer);

        // Copy prepared GPU data.
        auto copyTasks = ParallelTasks
        {
            TASK(CopyGpuTriangles2d(*copyPass))
        };
        executor.AddTasks(copyTasks).wait();

        SDL_EndGPUCopyPass(copyPass);

        // Begin render pass.
        auto colorTargetInfo = SDL_GPUColorTargetInfo
        {
            .texture  = _swapchainTexture,
            .load_op  = SDL_GPU_LOADOP_LOAD,
            .store_op = SDL_GPU_STOREOP_STORE
        };
        auto& renderPass = *SDL_BeginGPURenderPass(_commandBuffer, &colorTargetInfo, 1, nullptr);

        // 2D triangles.
        _gpuBuffers.Triangle2dVertices.Bind(renderPass, 0, 0);
        for (const auto& batch : _drawBatches.Triangles2d)
        {
            if (!batch.TextureName.empty())
            {
                auto& tex = GetTextures()[batch.TextureName];
                tex.Bind(renderPass, GetActiveSampler());

                _gpuBuffers.Triangle2dUni.UseTexture = true;
            }
            else
            {
                _gpuBuffers.Triangle2dUni.UseTexture = false;
            }

            _gpuBuffers.Triangle2dUni.IsFastAlpha = batch.BlendMd == BlendMode::FastAlpha;
            SDL_PushGPUFragmentUniformData(_commandBuffer, 0, &_gpuBuffers.Triangle2dUni, sizeof(_gpuBuffers.Triangle2dUni));

            _pipelines.Bind(renderPass, RenderStage::Triangle2d, batch.BlendMd);
            SDL_DrawGPUIndexedPrimitives(&renderPass, batch.BufferStride, 1, 0, batch.BufferOffset, 0);

            _doubleBuffer.Active.DrawCallCount++;
        }

        // End render pass.
        SDL_EndGPURenderPass(&renderPass);
    }

    void SdlGpuRenderer::DrawPostProcess()
    {
        const auto& options = g_App.GetOptions();

        // Begin render pass.
        auto colorTargetInfo = SDL_GPUColorTargetInfo
        {
            .texture  = _swapchainTexture,
            .load_op  = SDL_GPU_LOADOP_LOAD,
            .store_op = SDL_GPU_STOREOP_STORE
        };
        auto& renderPass = *SDL_BeginGPURenderPass(_commandBuffer, &colorTargetInfo, 1, nullptr);

        // Process render pass.

        // Dithering.
        if (options->EnableDithering)
        {
            // @todo
        }

        // Vignette.
        if (options->EnableVignette)
        {
            // @todo
        }

        // CRT filter.
        if (options->EnableCrtFilter)
        {
            // @todo
        }

        SDL_EndGPURenderPass(&renderPass);
    }

    void SdlGpuRenderer::DrawDebugGui()
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

    void SdlGpuRenderer::AllocateMemory()
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
        _drawBatches.Triangles2d.reserve(TRI_BATCH_COUNT_MAX);

        // Initialize GPU buffers.
        _gpuBuffers.Triangle2dVertices.Initialize(*_device, TRI_VERT_COUNT_MAX, TRI_IDX_COUNT_MAX, "2D triangle vertices");
    }

    void SdlGpuRenderer::ClearDrawBatches()
    {
        _drawBatches.Triangles2d.clear();
    }

    void SdlGpuRenderer::CopyGpuTriangles2d(SDL_GPUCopyPass& copyPass)
    {
        // Compute sizes.
        int sprite2dVertCount = (_doubleBuffer.Render.Sprites2d.size() * 2) * TRI_VERTEX_COUNT;
        int sprite2dIdxCount  = (_doubleBuffer.Render.Sprites2d.size() * 2) * TRI_IDX_COUNT;
        int shape2dVertCount  = (_doubleBuffer.Render.Shapes2d.size() * 2) * TRI_VERTEX_COUNT;
        int shape2dIdxCount   = (_doubleBuffer.Render.Shapes2d.size() * 2) * TRI_IDX_COUNT;

        // Create GPU buffer data.
        auto bufferVerts = std::vector<Vertex2dBuffer>{};
        auto bufferIdxs  = std::vector<uint16>{};

        bufferVerts.reserve(sprite2dVertCount + shape2dVertCount);
        bufferIdxs.reserve(sprite2dIdxCount + shape2dIdxCount);

        // Process 2D sprites.
        for (int i = 0; i < _doubleBuffer.Render.Sprites2d.size(); i++)
        {
            const auto& sprite = _doubleBuffer.Render.Sprites2d[i];

            // @todo Apply scale mode later.
            //auto pos = GetAspectCorrectScreenPosition(Vector2(vert.Position.x, vert.Position.y), sprite.ScaleMd);
            auto ndc = ConvertScreenPercentToNdc(sprite.Position);

            // Compute vertex positions.
            float depthZ = std::clamp((float)sprite.Depth / (float)DEPTH_MAX, 0.0f, 1.0f);
            auto  pos0   = Vector3(ndc.x - sprite.Scale.x, ndc.y + sprite.Scale.y, depthZ);
            auto  pos1   = Vector3(ndc.x + sprite.Scale.x, ndc.y + sprite.Scale.y, depthZ);
            auto  pos2   = Vector3(ndc.x + sprite.Scale.x, ndc.y - sprite.Scale.y, depthZ);
            auto  pos3   = Vector3(ndc.x - sprite.Scale.x, ndc.y - sprite.Scale.y, depthZ);

            // Compute vertex UVs.
            auto uv0 = sprite.UvMin;
            auto uv1 = Vector2(sprite.UvMax.x, sprite.UvMin.y);
            auto uv2 = sprite.UvMax;
            auto uv3 = Vector2(sprite.UvMin.x, sprite.UvMax.y);

            // Add vertices.
            bufferVerts.push_back(Vertex2dBuffer{ pos0, uv0, sprite.Col });
            bufferVerts.push_back(Vertex2dBuffer{ pos1, uv1, sprite.Col });
            bufferVerts.push_back(Vertex2dBuffer{ pos2, uv2, sprite.Col });
            bufferVerts.push_back(Vertex2dBuffer{ pos3, uv3, sprite.Col });

            // Add indices.
            bufferIdxs.push_back((i * (QUAD_IDX_COUNT)) + 0);
            bufferIdxs.push_back((i * (QUAD_IDX_COUNT)) + 1);
            bufferIdxs.push_back((i * (QUAD_IDX_COUNT)) + 2);
            bufferIdxs.push_back((i * (QUAD_IDX_COUNT)) + 0);
            bufferIdxs.push_back((i * (QUAD_IDX_COUNT)) + 2);
            bufferIdxs.push_back((i * (QUAD_IDX_COUNT)) + 3);

            // @todo Batching. For now, collect each as its own batch of 2 triangles.
            _drawBatches.Triangles2d.push_back(DrawBatch
            {
                .TextureName  = sprite.TextureName,
                .BlendMd      = sprite.BlendMd,
                .BufferOffset = i * QUAD_VERTEX_COUNT,
                .BufferStride = QUAD_IDX_COUNT
            });
        }

        // Process 2D shapes.
        int shape2dVertOffset = bufferVerts.size();
        for (const auto& shape : _doubleBuffer.Render.Shapes2d)
        {
            float depthZ       = std::clamp((float)shape.Depth / (float)DEPTH_MAX, 0.0f, 1.0f);
            int   curVertCount = 0;
            int   curIdxCount  = 0;

            // Triangle.
            if (shape.Vertices.size() == TRI_VERTEX_COUNT)
            {
                // Add vertices.
                for (const auto& vert : shape.Vertices)
                {
                    //auto pos = GetAspectCorrectScreenPosition(Vector2(vert.Position.x, vert.Position.y), prim.ScaleM);
                    auto ndc = ConvertScreenPercentToNdc(Vector2(vert.Position.x, vert.Position.y));
                    bufferVerts.push_back(Vertex2dBuffer{ Vector3(ndc.x, ndc.y, depthZ), Vector2::Zero, vert.Col });
                }

                // Add indices.
                bufferIdxs.push_back(shape2dVertOffset + 0);
                bufferIdxs.push_back(shape2dVertOffset + 1);
                bufferIdxs.push_back(shape2dVertOffset + 2);

                curVertCount = TRI_VERTEX_COUNT;
                curIdxCount  = TRI_IDX_COUNT;
            }
            // Line or quad.
            else if (shape.Vertices.size() == QUAD_VERTEX_COUNT)
            {
                // Add vertices.
                for (const auto& vert : shape.Vertices)
                {
                    //auto pos = GetAspectCorrectScreenPosition(Vector2(vert.Position.x, vert.Position.y), prim.ScaleM);
                    auto ndc = ConvertScreenPercentToNdc(Vector2(vert.Position.x, vert.Position.y));
                    bufferVerts.push_back(Vertex2dBuffer{ Vector3(ndc.x, ndc.y, depthZ), Vector2::Zero, vert.Col });
                }

                // Add indices.
                bufferIdxs.push_back(shape2dVertOffset + 0);
                bufferIdxs.push_back(shape2dVertOffset + 1);
                bufferIdxs.push_back(shape2dVertOffset + 2);
                bufferIdxs.push_back(shape2dVertOffset + 0);
                bufferIdxs.push_back(shape2dVertOffset + 2);
                bufferIdxs.push_back(shape2dVertOffset + 3);

                curVertCount = QUAD_VERTEX_COUNT;
                curIdxCount  = QUAD_IDX_COUNT;
            }

            // @todo Batching. For now, collect each as its own batch.
            _drawBatches.Triangles2d.push_back(DrawBatch
            {
                .TextureName  = {},
                .BlendMd      = shape.BlendMd,
                .BufferOffset = shape2dVertOffset,
                .BufferStride = curIdxCount
            });

            shape2dVertOffset += curVertCount;
        }

        // Update GPU buffer.
        // @lock Restrict GPU access.
        {
            auto lock = ParallelLock(_gpuMutex);
    
            _gpuBuffers.Triangle2dVertices.UpdateVertices(copyPass, ToSpan(bufferVerts), 0);
            _gpuBuffers.Triangle2dVertices.UpdateIdxs(copyPass, ToSpan(bufferIdxs), 0);
        }
    }
}
