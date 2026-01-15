#pragma once

#include "Renderer/Backends/SdlGpu/Gpu/Buffer.h"
#include "Renderer/Backends/SdlGpu/Gpu/Layouts/Triangle2dUniform.h"
#include "Renderer/Backends/SdlGpu/Gpu/Layouts/Vertex2dBuffer.h"
#include "Renderer/Backends/SdlGpu/Gpu/VertexBuffer.h"
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
        int         BufferOffset = 0;
        int         BufferStride = 0;
    };

    /** @brief Sorted draw batches. */
    struct DrawBatches
    {
        std::vector<DrawBatch> Triangles2d = {};
    };

    /** @brief GPU buffers. */
    struct GpuBuffers
    {
        VertexBuffer<Vertex2dBuffer> Triangle2dVertices = {};

        Triangle2dUni Triangle2dUni = {};
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

        /** @brief Allocates memory pools for for draw batches and GPU buffers. */
        void AllocateMemory();

        /** @brief Clears draw batches for reuse. */
        void ClearDrawBatches();

        /** @brief Converts render buffer data to 2D triangle GPU buffer data and uploads it to the GPU.
         *
         * Processes 2D sprites and shapes.
         *
         * @param copyPass Copy pass.
         */
        void CopyGpuTriangles2d(SDL_GPUCopyPass& copyPass);
    };
}
