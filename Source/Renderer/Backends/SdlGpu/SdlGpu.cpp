#include "Framework.h"
#include "Renderer/Backends/SdlGpu/SdlGpu.h"

#include "Application.h"
#include "Assets/Fonts.h"
#include "Renderer/Backends/SdlGpu/Objects/Texture.h"
#include "Renderer/Backends/SdlGpu/Pipeline/Pipeline.h"
#include "Renderer/Common/Objects/Buffers.h"
#include "Renderer/Common/Objects/Uniforms.h"
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

        InitializeBuffers();

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

        //_textures TestTexture.~Texture();
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
            Draw3dScene();
            Draw2dScene();
            DrawPostProcess();
            DrawDebugGui();
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

    void Renderer::Draw3dScene()
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

    TextureManager& Renderer::GetTextures()
    {
        return *(TextureManager*)_textures.get();
    }

    SDL_GPUSampler& Renderer::GetActiveSampler()
    {
        const auto& options = g_App.GetOptions();

        return *_samplers[(int)options->TextureFilter];
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
            .texture  = _swapchainTexture,
            .load_op  = SDL_GPU_LOADOP_LOAD,
            .store_op = SDL_GPU_STOREOP_STORE
        };
        auto& renderPass = *SDL_BeginGPURenderPass(_commandBuffer, &colorTargetInfo, 1, nullptr);

        // 2D triangles.
        _gpuBuffers.Vertices2d.Bind(renderPass, 0, 0);
        for (const auto& batch : _drawBatches.Primitives2d)
        {
            if (!batch.TextureName.empty())
            {
                auto* tex = GetTextures()[batch.TextureName];
                if (tex != nullptr)
                {
                    tex->Bind(renderPass, GetActiveSampler());
                }
            }

            _pipelines.Bind(renderPass, batch.RenderStg, batch.BlendMd);
            PushFragmentUniform(batch.Uniform, 0);

            SDL_DrawGPUIndexedPrimitives(&renderPass, batch.BufferStride, 1, 0, batch.BufferOffset, 0);

            _doubleBuffer.Active.DrawCallCount++;
        }

        // End render pass.
        SDL_EndGPURenderPass(&renderPass);
    }

    void Renderer::DrawPostProcess()
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

    void Renderer::DrawDebugGui()
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

    void Renderer::InitializeBuffers()
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
        // Compute sizes.
        int sprite2dVertCount = _doubleBuffer.Render.Sprites2d.size() * QUAD_VERTEX_COUNT;
        int sprite2dIdxCount  = _doubleBuffer.Render.Sprites2d.size() * (TRI_IDX_COUNT * 2);
        int shape2dVertCount  = _doubleBuffer.Render.Shapes2d.size() * QUAD_VERTEX_COUNT;
        int shape2dIdxCount   = _doubleBuffer.Render.Shapes2d.size() * (TRI_IDX_COUNT * 2);
        int glyph2dVertCount  = _doubleBuffer.Render.Glyphs2d.size() * QUAD_VERTEX_COUNT;
        int glyph2dIdxCount   = _doubleBuffer.Render.Glyphs2d.size() * (TRI_IDX_COUNT * 2);

        // @todo Before processing into batched GPU data, combine these into an intermediate collection.
        _doubleBuffer.Render.Primitives2d.clear(); // @todo
        _doubleBuffer.Render.Primitives2d.reserve(_doubleBuffer.Render.Sprites2d.size() + _doubleBuffer.Render.Shapes2d.size() + _doubleBuffer.Render.Glyphs2d.size());

        // Process 2D sprites.
        for (int i = 0; i < _doubleBuffer.Render.Sprites2d.size(); i++)
        {
            const auto& sprite = _doubleBuffer.Render.Sprites2d[i];

            // @todo Apply scale mode later.
            //auto pos = GetAspectCorrectScreenPosition(Vector2(vert.Position.x, vert.Position.y), sprite.ScaleMd);
            auto ndc = ConvertScreenPercentToNdc(sprite.Position);

            // Set alignment offset.
            auto offset = Vector2::Zero;
            switch (sprite.AlignMd)
            {
                default:
                case AlignMode::Center:
                {
                    break;
                }
                case AlignMode::CenterTop:
                {
                    offset = Vector2(0.0f, -sprite.Scale.y);
                    break;
                }
                case AlignMode::CenterBottom:
                {
                    offset = Vector2(0.0f, sprite.Scale.y);
                    break;
                }
                case AlignMode::CenterLeft:
                {
                    offset = Vector2(sprite.Scale.x, 0.0f);
                    break;
                }
                case AlignMode::CenterRight:
                {
                    offset = Vector2(-sprite.Scale.x, 0.0f);
                    break;
                }
                case AlignMode::TopLeft:
                {
                    offset = Vector2(sprite.Scale.x, -sprite.Scale.y);
                    break;
                }
                case AlignMode::TopRight:
                {
                    offset = Vector2(-sprite.Scale.x, -sprite.Scale.y);
                    break;
                }
                case AlignMode::BottomLeft:
                {
                    offset = Vector2(sprite.Scale.x, sprite.Scale.y);
                    break;
                }
                case AlignMode::BottomRight:
                {
                    offset = Vector2(-sprite.Scale.x, sprite.Scale.y);
                    break;
                }
            }

            // Compute relative vertex positions.
            auto rotMat  = Matrix::CreateRotationZ(-sprite.Rotation);
            auto relPos0 = Vector2::Transform(Vector2(-sprite.Scale.x, sprite.Scale.y) + offset, rotMat);
            auto relPos1 = Vector2::Transform(sprite.Scale                             + offset, rotMat);
            auto relPos2 = Vector2::Transform(Vector2(sprite.Scale.x, -sprite.Scale.y) + offset, rotMat);
            auto relPos3 = Vector2::Transform(-sprite.Scale                            + offset, rotMat);

            // Compute vertex positions.
            auto pos0 = Vector2(ndc.x + relPos0.x, ndc.y + relPos0.y);
            auto pos1 = Vector2(ndc.x + relPos1.x, ndc.y + relPos1.y);
            auto pos2 = Vector2(ndc.x + relPos2.x, ndc.y + relPos2.y);
            auto pos3 = Vector2(ndc.x + relPos3.x, ndc.y + relPos3.y);

            // Compute vertex UVs.
            auto uv0 = sprite.UvMin;
            auto uv1 = Vector2(sprite.UvMax.x, sprite.UvMin.y);
            auto uv2 = sprite.UvMax;
            auto uv3 = Vector2(sprite.UvMin.x, sprite.UvMax.y);

            // Add 2D primitive.
            _doubleBuffer.Render.Primitives2d.push_back(Primitive2d
            {
                .Vertices =
                {
                    { pos0, sprite.Col0, uv0 },
                    { pos1, sprite.Col1, uv1 },
                    { pos2, sprite.Col2, uv2 },
                    { pos3, sprite.Col3, uv3 }
                },
                .Depth       = sprite.Depth,
                .TextureName = sprite.TextureName,
                .RenderStg   = RenderStage::Sprite2d,
                .BlendMd     = sprite.BlendMd,
                .Uniform     = UniformSprite2d
                {
                    .UseTexture  = true, 
                    .IsFastAlpha = sprite.BlendMd == BlendMode::FastAlpha
                }
            });
        }

        // Process 2D shapes.
        for (const auto& shape : _doubleBuffer.Render.Shapes2d)
        {
            // Triangle.
            if (shape.Vertices.size() == TRI_VERTEX_COUNT)
            {
                // Compute vertex positions.
                auto pos0 = ConvertScreenPercentToNdc(Vector2(shape.Vertices[0].Position.x, shape.Vertices[0].Position.y));
                auto pos1 = ConvertScreenPercentToNdc(Vector2(shape.Vertices[1].Position.x, shape.Vertices[1].Position.y));
                auto pos2 = ConvertScreenPercentToNdc(Vector2(shape.Vertices[2].Position.x, shape.Vertices[2].Position.y));

                // Add 2D primitive.
                _doubleBuffer.Render.Primitives2d.push_back(Primitive2d
                {
                    .Vertices =
                    {
                        Vertex2d{ pos0, shape.Vertices[0].Col, Vector2::Zero },
                        Vertex2d{ pos1, shape.Vertices[1].Col, Vector2::Zero },
                        Vertex2d{ pos2, shape.Vertices[2].Col, Vector2::Zero },
                    },
                    .Depth       = shape.Depth,
                    .TextureName = {},
                    .RenderStg   = RenderStage::Sprite2d,
                    .BlendMd     = shape.BlendMd,
                    .Uniform     = UniformSprite2d
                    {
                        .UseTexture  = false, 
                        .IsFastAlpha = shape.BlendMd == BlendMode::FastAlpha
                    }
                });
            }
            // Line or quad.
            else if (shape.Vertices.size() == QUAD_VERTEX_COUNT)
            {
                // Compute vertex positions.
                auto pos0 = ConvertScreenPercentToNdc(Vector2(shape.Vertices[0].Position.x, shape.Vertices[0].Position.y));
                auto pos1 = ConvertScreenPercentToNdc(Vector2(shape.Vertices[1].Position.x, shape.Vertices[1].Position.y));
                auto pos2 = ConvertScreenPercentToNdc(Vector2(shape.Vertices[2].Position.x, shape.Vertices[2].Position.y));
                auto pos3 = ConvertScreenPercentToNdc(Vector2(shape.Vertices[3].Position.x, shape.Vertices[3].Position.y));

                // Add 2D primitive.
                _doubleBuffer.Render.Primitives2d.push_back(Primitive2d
                {
                    .Vertices =
                    {
                        Vertex2d{ pos0, shape.Vertices[0].Col, Vector2::Zero },
                        Vertex2d{ pos1, shape.Vertices[1].Col, Vector2::Zero },
                        Vertex2d{ pos2, shape.Vertices[2].Col, Vector2::Zero },
                        Vertex2d{ pos3, shape.Vertices[3].Col, Vector2::Zero }
                    },
                    .Depth       = shape.Depth,
                    .TextureName = {},
                    .RenderStg   = RenderStage::Sprite2d,
                    .BlendMd     = shape.BlendMd,
                    .Uniform     = UniformSprite2d
                    {
                        .UseTexture  = false, 
                        .IsFastAlpha = shape.BlendMd == BlendMode::FastAlpha
                    }
                });
            }
        }

        // Process 2D glyphs.
        for (const auto& glyph : _doubleBuffer.Render.Glyphs2d)
        {
            // @todo Apply scale mode later.
            //auto pos = GetAspectCorrectScreenPosition(Vector2(vert.Position.x, vert.Position.y), sprite.ScaleMd);
            auto ndc = ConvertScreenPercentToNdc(glyph.Position);

            // Set alignment offset.
            auto offset = Vector2(glyph.Scale.x, glyph.Scale.y);

            // Compute relative vertex positions.
            auto rotMat  = Matrix::CreateRotationZ(-glyph.Rotation);
            auto relPos0 = Vector2::Transform(Vector2(-glyph.Scale.x, glyph.Scale.y) + offset, rotMat);
            auto relPos1 = Vector2::Transform(glyph.Scale                            + offset, rotMat);
            auto relPos2 = Vector2::Transform(Vector2(glyph.Scale.x, -glyph.Scale.y) + offset, rotMat);
            auto relPos3 = Vector2::Transform(-glyph.Scale                           + offset, rotMat);

            // Compute vertex positions.
            auto pos0 = ndc + relPos0;
            auto pos1 = ndc + relPos1;
            auto pos2 = ndc + relPos2;
            auto pos3 = ndc + relPos3;

            // Compute vertex UVs.
            auto uv0 = glyph.UvMin;
            auto uv1 = Vector2(glyph.UvMax.x, glyph.UvMin.y);
            auto uv2 = glyph.UvMax;
            auto uv3 = Vector2(glyph.UvMin.x, glyph.UvMax.y);

            // Add 2D primitive.
            _doubleBuffer.Render.Primitives2d.push_back(Primitive2d
            {
                .Vertices =
                {
                    Vertex2d{ pos0, glyph.Col, uv0 },
                    Vertex2d{ pos1, glyph.Col, uv1 },
                    Vertex2d{ pos2, glyph.Col, uv2 },
                    Vertex2d{ pos3, glyph.Col, uv3 }
                },
                .Depth       = glyph.Depth,
                .TextureName = glyph.AtlasName,
                .RenderStg   = RenderStage::Glyph2d,
                .BlendMd     = BlendMode::Alpha,
                .Uniform     = UniformGlyph2d
                {
                    .HasGradient    = glyph.HasGradient,
                    .GradientSteps  = (uint)glyph.GradientSteps,
                    .GradientUvMinY = glyph.GradientUvMinY,
                    .GradientUvMaxY = glyph.GradientUvMaxY
                }
            });
        }

        // Sort 2D primitives.
        Sort(_doubleBuffer.Render.Primitives2d, [](const Primitive2d& prim0, const Primitive2d& prim1)
        {
            return prim0.Depth > prim1.Depth;
        });

        auto bufferVerts = std::vector<BufferVertex2d>{};
        auto bufferIdxs  = std::vector<uint16>{};

        bufferVerts.reserve(sprite2dVertCount + shape2dVertCount + glyph2dVertCount);
        bufferIdxs.reserve(sprite2dIdxCount + shape2dIdxCount + glyph2dIdxCount);

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
            // @todo Smarter way that strings together primitives with the same render stage, blend mode, and texture. Uniform?
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
