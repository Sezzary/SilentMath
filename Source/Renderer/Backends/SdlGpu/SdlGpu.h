#pragma once

#include "Renderer/Backends/SdlGpu/Buffer/Buffer.h"
#include "Renderer/Backends/SdlGpu/Buffer/VertexBuffer.h"
#include "Renderer/Backends/SdlGpu/Pipeline.h"
#include "Renderer/Backends/SdlGpu/Texture.h"
#include "Renderer/Common/Objects/Primitive/Vertex2d.h"
#include "Renderer/Common/Objects/Primitive/Vertex3d.h"
#include "Renderer/Renderer.h"

// @todo Temp. placement.
#include "Renderer/Backends/SdlGpu/Buffer/Data/BufferColorVertex2d.h"
#include "Renderer/Backends/SdlGpu/Buffer/Data/BufferTexVertex2d.h"

namespace Silent::Renderer
{
    struct BufferData
    {
        Buffer<BufferColorVertex2d>     Primitives2d = {};
        VertexBuffer<BufferTexVertex2d> Sprites2d    = {};
    };

    class SdlGpuRenderer : public RendererBase
    {
    private:
        // =======
        // Fields
        // =======

        SDL_GPUDevice*               _device           = nullptr;
        SDL_GPUTexture*              _swapchainTexture = nullptr;
        SDL_GPUCommandBuffer*        _commandBuffer    = nullptr;
        std::vector<SDL_GPUSampler*> _samplers         = {};
        BufferData                   _buffers          = {};
        PipelineManager              _pipelines        = PipelineManager();

    public:
        // =============
        // Constructors
        // =============

        /** @brief Constructs an uninitialized default `SdlGpuRenderer`. */
        SdlGpuRenderer() = default;

        // ==========
        // Utilities
        // ==========

        void Initialize(SDL_Window& window) override;
        void Deinitialize() override;
        void Update() override;
        void SaveScreenshot() const override;

    private:
        // ========
        // Helpers
        // ========

        SdlGpuTextureManager& GetTextures();

        void Draw3dScene() override;
        void Draw2dScene() override;
        void DrawPostProcess() override;
        void DrawDebugGui() override;

        void Copy2dPrimitives(SDL_GPUCopyPass& copyPass, std::vector<BufferColorVertex2d>& bufferVerts);
        void Copy2dSprites(SDL_GPUCopyPass& copyPass, std::vector<BufferTexVertex2d>& bufferVerts, std::vector<uint16>& bufferIdxs);
    };
}
