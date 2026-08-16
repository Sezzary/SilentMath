#include "Framework.h"
#include "Renderer/Backends/SdlGpu/SdlGpu.h"

#include "Application.h"
#include "Assets/AssetStreamer.h"
#include "Renderer/Backends/SdlGpu/Pipeline/Pipeline.h"
#include "Renderer/Backends/SdlGpu/Resources/Buffer.h"
#include "Renderer/Backends/SdlGpu/Resources/MeshCache.h"
#include "Renderer/Backends/SdlGpu/Resources/PingPongTexture.h"
#include "Renderer/Backends/SdlGpu/Resources/TextureCache.h"
#include "Renderer/Backends/SdlGpu/Resources/VertexBuffer.h"
#include "Renderer/Common/View.h"
#include "Renderer/Renderer.h"
#include "Services/Options.h"

using namespace Silent::Assets;
using namespace Silent::Services;

namespace Silent::Renderer::SdlGpu
{
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

        GetMeshes().Bind(renderPass);
        _pipelines.Bind(renderPass, RenderStage::Model, BlendMode::Alpha);

        _view.Move();

        // @temp
        //---------------------------

        const auto* harryAnm = g_App.GetAssets()["ANIM/HB_BASE.ANM"];
        const auto harryAnmData = harryAnm->GetData<AnmAsset>();

        // @todo How can an optional palette be bound in a clean way?
        auto* tex             = GetTextures()["CHARA/HERO.TIM"];
        auto* paletteAtlasTex = GetTextures()[Fmt("{}{}", "CHARA/HERO.TIM", PALETTE_ATLAS_SUFFIX)];
        if (tex != nullptr)
        {
            tex->Bind(renderPass, GetActiveSampler(), 0);
            paletteAtlasTex->Bind(renderPass, GetActiveSampler(), 1);

            auto model = Matrix::Identity;
            model.Rotate(DEG_TO_RAD(180.0f), Vector3::UnitX);
            model.Translate(harryAnmData->Bones[1].BindTranslation);

            auto viewProj = _view.GetMatrix(glm::radians(0.0f), GetViewportAspectRatio(), 0.1f, 100.0f);

            auto uni0 = UniformView{};
            memcpy(&uni0.ViewProjMat, &viewProj[0][0], 64);
            PushVertexUniform(uni0, 0);

            auto uni1 = UniformPrimitive3d{};
            memcpy(&uni1.ModelMat, &model[0][0], 64);
            PushVertexUniform(uni1, 1);

            // Push uniform.
            auto uni = UniformModel
            {
                .IsFastAlpha = false
            };
            PushFragmentUniform(uni, 0);

            // Draw.
            const auto* mesh = GetMeshes()["CHARA/HERO.ILM_02HEAD1"];
            if (mesh != nullptr)
            {
                SDL_DrawGPUIndexedPrimitives(&renderPass, mesh->IdxCount, 1, mesh->IdxOffset, mesh->VertexOffset, 0);
                _doubleBuffer.Active.DrawCallCount++;
            }
        }

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
                tex->Bind(renderPass, GetActiveSampler(), 0);
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
            _pipelines.Bind(renderPass, RenderStage::Dither, BlendMode::Opaque, false);

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

        // Bind default texture and palette atlas.
        auto* tex = GetTextures()[""];
        tex->Bind(renderPass, GetActiveSampler(), 0);
        tex->Bind(renderPass, GetActiveSampler(), 1);

        // Draw 2D primitives.
        _gpuBuffers.ImmediateVertices2d.Bind(renderPass, 0, 0);
        for (const auto& batch : _drawBatches.Primitives2d)
        {
            // Bind pipeline.
            _pipelines.Bind(renderPass, batch.RenderStg, batch.BlendMd);
            PushFragmentUniform(batch.Uniform, 0);

            // Bind texture and optional palette atlas.
            if (!batch.TextureName.empty())
            {
                tex = GetTextures()[batch.TextureName];
                if (tex != nullptr)
                {
                    tex->Bind(renderPass, GetActiveSampler(), 0);
                }
                tex = GetTextures()[batch.TextureName + PALETTE_ATLAS_SUFFIX];
                if (tex != nullptr)
                {
                    tex->Bind(renderPass, GetActiveSampler(), 1);
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
            _pipelines.Bind(*renderPass, renderStage, BlendMode::Opaque, false);

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

        // @debug Luma fade test.
        if (Debug::g_Work.BlendAlpha > 0.0f)
        {
            RunPostProcessPass(RenderStage::LumaFade, [&]()
            {
                auto uni = UniformLumaFade
                {
                    .FadeAlpha = Debug::g_Work.BlendAlpha,
                    .IsWhite   = false
                };
                PushFragmentUniform(uni, 0);
            });
        }

        // Film grain.
        static float time = 0.0f; // @debug Test timer.
        time = fmod(time + 0.1f, 1.0f);
        if (options->EnableFilmGrain)
        {
            RunPostProcessPass(RenderStage::FilmGrain, [&]()
            {
                auto uni = UniformFilmGrain
                {
                    .Time = time
                };
                PushFragmentUniform(uni, 0);
            });
        }

        // Vignette.
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

        // CRT filter.
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

        _pipelines.Bind(renderPass, RenderStage::Blit, BlendMode::Opaque, false);

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

    void Renderer::DrawDebugMenu()
    {
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
}
