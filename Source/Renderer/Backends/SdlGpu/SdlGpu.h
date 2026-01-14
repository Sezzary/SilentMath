#pragma once

#include "Renderer/Backends/SdlGpu/Buffer/Buffer.h"
#include "Renderer/Backends/SdlGpu/Buffer/Layouts/Vertex2dBuffer.h"
#include "Renderer/Backends/SdlGpu/Buffer/VertexBuffer.h"
#include "Renderer/Backends/SdlGpu/Pipeline.h"
#include "Renderer/Backends/SdlGpu/Texture.h"
#include "Renderer/Common/Objects/Primitive/Vertex2d.h"
#include "Renderer/Common/Objects/Primitive/Vertex3d.h"
#include "Renderer/Renderer.h"

namespace Silent::Renderer
{
    /** @brief Renderer draw batch. */
    struct DrawBatch
    {
        std::string TextureName  = {};
        BlendMode   BlendMd      = BlendMode::Opaque;
        int         BufferStride = 0;
        int         BufferOffset = 0;
    };

    /** @brief Sorted draw batches. */
    struct DrawBatches
    {
        std::vector<DrawBatch> Triangles2d = {};
    };

    /** @brief GPU buffers. */
    struct GpuBuffers
    {
        Buffer<Vertex2dBuffer>       Primitives2d = {};
        VertexBuffer<Vertex2dBuffer> Triangles2d    = {};
    };

    /** SDL_gpu renderer backend. */
    class SdlGpuRenderer : public RendererBase
    {
    public:
        // ==========
        // Constants
        // ==========

        static constexpr char NAME[] = "SDL_gpu";

    private:
        // =======
        // Fields
        // =======

        SDL_GPUDevice*               _device    = nullptr;
        std::vector<SDL_GPUSampler*> _samplers  = {};
        PipelineManager              _pipelines = PipelineManager();
        
        SDL_GPUTexture*       _swapchainTexture = nullptr;
        SDL_GPUCommandBuffer* _commandBuffer    = nullptr;
        DrawBatches           _drawBatches      = {};
        GpuBuffers            _gpuBuffers       = {};

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
        SDL_GPUSampler&       GetActiveSampler();

        void Draw3dScene() override;
        void Draw2dScene() override;
        void DrawPostProcess() override;
        void DrawDebugGui() override;

        // -----------------------
        // `SdlGpuBufferCopy.cpp`
        // -----------------------

        /** @brief Converts render buffer data for 2D primitives to the corresponding GPU buffer data
         * and uploads it to the GPU.
         *
         * @param copyPass Copy pass.
         * @param bufferVerts Output GPU buffer vertices.
         */
        void Copy2dPrimitives(SDL_GPUCopyPass& copyPass, std::vector<Vertex2dBuffer>& bufferVerts);

        /** @brief Converts render buffer data for 2D sprites to the corresponding GPU buffer data
         * and uploads it to the GPU.
         *
         * @param copyPass Copy pass.
         * @param bufferVerts Output GPU buffer vertices.
         * @param bufferIdxs Output GPU buffer indices.
         */
        void Copy2dSprites(SDL_GPUCopyPass& copyPass, std::vector<Vertex2dBuffer>& bufferVerts, std::vector<uint16>& bufferIdxs);
    };
}
