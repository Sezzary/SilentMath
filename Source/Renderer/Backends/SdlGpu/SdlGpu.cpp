#include "Framework.h"
#include "Renderer/Backends/SdlGpu/SdlGpu.h"

#include "Application.h"
#include "Renderer/Backends/SdlGpu/Buffer/Buffer.h"
#include "Renderer/Backends/SdlGpu/Buffer/VertexBuffer.h"
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
    struct TestUniform
    {
        bool IsFastAlpha = false;
    };

    static auto UniformBuffer = TestUniform{};

    void SdlGpuRenderer::Initialize(SDL_Window& window)
    {
        Debug::Log("Using SDL_gpu renderer.");

        _type   = RendererType::SdlGpu;
        _window = &window;

        // Define format flags.
        int formatFlags = 0;
        switch (OS_TYPE)
        {
            case OsType::Windows:
            {
                formatFlags = SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL;
                break;
            }
            case OsType::MacOs:
            {
                formatFlags = SDL_GPU_SHADERFORMAT_MSL;
                break;
            }
            case OsType::Linux:
            {
                formatFlags = SDL_GPU_SHADERFORMAT_SPIRV;
                break;
            }
        }

        // Create GPU device.
        _device = SDL_CreateGPUDevice(formatFlags, Debug::IS_DEBUG_BUILD, nullptr);
        if (_device == nullptr)
        {
            throw std::runtime_error(Fmt("Failed to create GPU device: {}", SDL_GetError()));
        }

        const char* deviceDriverName = SDL_GetGPUDeviceDriver(_device);
        if (deviceDriverName != nullptr)
        {
            Debug::Log(Fmt("Using {} backend.", deviceDriverName));
        }

        // Claim window.
        if (!SDL_ClaimWindowForGPUDevice(_device, _window))
        {
            throw std::runtime_error(Fmt("Failed to claim window for GPU device: {}", SDL_GetError()));
        }

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

        // Initialize GPU buffers.
        _buffers.Primitives2d.Initialize(*_device, SDL_GPU_BUFFERUSAGE_VERTEX, (PRIMITIVE_2D_COUNT_MAX * 2) * TRIANGLE_VERTEX_COUNT, "2d primitive triangle vertices");
        _buffers.Sprites2d.Initialize(*_device, SPRITE_2D_COUNT_MAX * QUAD_VERTEX_COUNT, SPRITE_2D_COUNT_MAX * 6, "2D sprite vertices");

        // Reserve memory.
        _activeBuffer.Primitives2d.reserve(PRIMITIVE_2D_COUNT_MAX);
        _activeBuffer.Sprites2d.reserve(SPRITE_2D_COUNT_MAX);
        _renderBuffer = _activeBuffer; // @todo Check if this reserves memory like it's supposed to. `_activeBuffer` and `_renderBuffer` should have identical memory pools.

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

        GetTextures().Load(*copyPass, g_App.GetAssets().GetName(1854));

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
        _buffers = {};
        _pipelines.Deinitialize();

        SDL_ReleaseWindowFromGPUDevice(_device, _window);
        SDL_DestroyGPUDevice(_device);
    }

    void SdlGpuRenderer::Update()
    {
        // Frame setup.
        PrepareRenderBufferData();

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
        constexpr uint COLOR_CHANNEL_COUNT = 3; // RGB.

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

    void SdlGpuRenderer::Draw2dScene()
    {
        const auto& options = g_App.GetOptions();

        // Process copy pass.
        auto* copyPass = SDL_BeginGPUCopyPass(_commandBuffer);

        auto bufferVerts = std::vector<BufferColorVertex2d>{};
        Copy2dPrimitives(*copyPass, bufferVerts);

        auto spriteBufferVerts = std::vector<BufferTexVertex2d>{};
        auto spriteBufferIdxs  = std::vector<uint16>{};
        Copy2dSprites(*copyPass, spriteBufferVerts, spriteBufferIdxs);

        SDL_EndGPUCopyPass(copyPass);

        // Begin render pass.
        auto colorTargetInfo = SDL_GPUColorTargetInfo
        {
            .texture  = _swapchainTexture,
            .load_op  = SDL_GPU_LOADOP_LOAD,
            .store_op = SDL_GPU_STOREOP_STORE
        };
        auto& renderPass = *SDL_BeginGPURenderPass(_commandBuffer, &colorTargetInfo, 1, nullptr);

        // 2D sprites. @todo 6 indices should be a constant.
        _pipelines.Bind(renderPass, RenderStage::Primitive2dTextured, BlendMode::Opaque);
        _buffers.Sprites2d.Bind(renderPass, 0, 0);
        GetTextures().Get(g_App.GetAssets().GetName(1854))->Bind(renderPass, *_samplers[(int)options->TextureFilter]);
        SDL_DrawGPUIndexedPrimitives(&renderPass, 6, sizeof(spriteBufferVerts) / sizeof(BufferTexVertex2d), 0, 0, 0);

        // 2D primitives.
        _pipelines.Bind(renderPass, RenderStage::Primitive2d, BlendMode::Alpha);
        _buffers.Primitives2d.Bind(renderPass, 0);
        UniformBuffer.IsFastAlpha = false;
        SDL_PushGPUFragmentUniformData(_commandBuffer, 0, &UniformBuffer, sizeof(UniformBuffer));
        SDL_DrawGPUPrimitives(&renderPass, bufferVerts.size(), sizeof(bufferVerts) / sizeof(BufferColorVertex2d), 0, 0);
        
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
        for (auto& drawCall : _renderBuffer.DebugGuiDrawCalls)
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

    void SdlGpuRenderer::Copy2dPrimitives(SDL_GPUCopyPass& copyPass, std::vector<BufferColorVertex2d>& bufferVerts)
    {
        // Create 2D primitive vertex buffer data.
        bufferVerts.reserve((_renderBuffer.Primitives2d.size() * 2) * TRIANGLE_VERTEX_COUNT);
        for (const auto& prim : _renderBuffer.Primitives2d)
        {
            // 2D triangle primitive.
            if (prim.Vertices.size() == TRIANGLE_VERTEX_COUNT)
            {
                for (const auto& vert : prim.Vertices)
                {
                    //auto pos = GetAspectCorrectScreenPosition(Vector2(vert.Position.x, vert.Position.y), prim.ScaleM);
                    auto ndc = ConvertScreenPercentToNdc(Vector2(vert.Position.x, vert.Position.y));
                    bufferVerts.push_back(BufferColorVertex2d
                    {
                        .Position = Vector3(ndc.x, ndc.y, std::clamp((float)prim.Depth / (float)DEPTH_MAX, 0.0f, 1.0f)),
                        .Col      = vert.Col
                    });
                }
            }
            // 2D line or quad primitive.
            else if (prim.Vertices.size() == QUAD_VERTEX_COUNT)
            {
                for (int i : QUAD_TRIANGLE_IDXS)
                {
                    const auto& vert = prim.Vertices[i];

                    //auto pos = GetAspectCorrectScreenPosition(Vector2(vert.Position.x, vert.Position.y), prim.ScaleM);
                    auto ndc = ConvertScreenPercentToNdc(Vector2(vert.Position.x, vert.Position.y));
                    bufferVerts.push_back(BufferColorVertex2d
                    {
                        .Position = Vector3(ndc.x, ndc.y, std::clamp((float)prim.Depth / (float)DEPTH_MAX, 0.0f, 1.0f)),
                        .Col      = vert.Col
                    });
                }
            }
        }

        // Update buffer.
        _buffers.Primitives2d.Update(copyPass, ToSpan(bufferVerts), 0);
    }

    void SdlGpuRenderer::Copy2dSprites(SDL_GPUCopyPass& copyPass, std::vector<BufferTexVertex2d>& bufferVerts, std::vector<uint16>& bufferIdxs)
    {
        // Create 2D sprite vertex buffer data.
        bufferVerts.reserve(_renderBuffer.Sprites2d.size() * 4);
        bufferIdxs.reserve(_renderBuffer.Sprites2d.size() * 6);
        for (const auto& sprite : _renderBuffer.Sprites2d)
        {
            //auto pos = GetAspectCorrectScreenPosition(Vector2(vert.Position.x, vert.Position.y), prim.ScaleM);
            auto ndc = ConvertScreenPercentToNdc(sprite.Position);
            /*for (int i = 0; i < 4; i++)
            {
                bufferVerts.push_back(BufferTexVertex2d
                {
                    .x = ndc.x,
                    .y = ndc.y,
                    .z = 0.0f,
                    .u = sprite.UvMin,
                    .v = ndc.v
                });
            }*/

            bufferVerts.push_back(BufferTexVertex2d{ Vector3(-1.0f,  1.0f, 0.0f), Vector2(0.0f, 0.0f) });
            bufferVerts.push_back(BufferTexVertex2d{ Vector3( 1.0f,  1.0f, 0.0f), Vector2(1.0f, 0.0f) });
            bufferVerts.push_back(BufferTexVertex2d{ Vector3( 1.0f, -1.0f, 0.0f), Vector2(1.0f, 1.0f) });
            bufferVerts.push_back(BufferTexVertex2d{ Vector3(-1.0f, -1.0f, 0.0f), Vector2(0.0f, 1.0f) });

            bufferIdxs.push_back(0);
            bufferIdxs.push_back(1);
            bufferIdxs.push_back(2);
            bufferIdxs.push_back(0);
            bufferIdxs.push_back(2);
            bufferIdxs.push_back(3);
        }

        // Update buffer.
        _buffers.Sprites2d.UpdateVertices(copyPass, ToSpan(bufferVerts), 0);
        _buffers.Sprites2d.UpdateIdxs(copyPass, ToSpan(bufferIdxs), 0);
    }
}
