#pragma once

#include "Renderer/Backends/SdlGpu/Pipeline/Pipeline.h"
#include "Renderer/Backends/SdlGpu/Resources/Buffer.h"
#include "Renderer/Backends/SdlGpu/Resources/MeshCache.h"
#include "Renderer/Backends/SdlGpu/Resources/PingPongTexture.h"
#include "Renderer/Backends/SdlGpu/Resources/TextureCache.h"
#include "Renderer/Backends/SdlGpu/Resources/VertexBuffer.h"
#include "Renderer/Common/Resources/Buffers.h"
#include "Renderer/Common/Resources/Primitive/Vertex2d.h"
#include "Renderer/Common/Resources/Primitive/Vertex3d.h"
#include "Renderer/Common/Resources/Uniforms.h"
#include "Renderer/Renderer.h"

namespace Silent::Renderer::SdlGpu
{
    /** @brief GPU buffer draw batch. */
    struct DrawBatch
    {
        std::string TextureName  = {};
        RenderStage RenderStg    = RenderStage::Sprite2d;
        BlendMode   BlendMd      = BlendMode::Opaque;
        UniformType Uniform      = {};
        int         VertexCount  = 0;
        int         VertexOffset = 0;
        int         IdxOffset    = 0;
    };

    /** @brief Sorted GPU buffer draw batches. */
    struct DrawBatches
    {
        std::vector<DrawBatch> Primitives2d = {};
        std::vector<DrawBatch> Primitives3d = {};
    };

    /** @brief GPU buffers. */
    struct GpuBuffers
    {
        VertexBuffer<BufferVertex2d> ViewportVertices    = {};
        VertexBuffer<BufferVertex2d> ImmediateVertices2d = {};
        VertexBuffer<BufferVertex3d> ImmediateVertices3d = {};
    };

    /** @brief SDL_gpu renderer backend. */
    class Renderer : public RendererBase
    {
    private:
        // =======
        // Fields
        // =======

        SDL_GPUDevice*               _device    = nullptr;
        std::vector<SDL_GPUSampler*> _samplers  = {};
        PipelineManager              _pipelines = PipelineManager();

        PingPongTexture       _renderTexture    = PingPongTexture();
        SDL_GPUTexture*       _depthTexture     = nullptr;
        SDL_GPUTexture*       _swapchainTexture = nullptr;
        SDL_GPUCommandBuffer* _commandBuffer    = nullptr;
        DrawBatches           _drawBatches      = {};
        GpuBuffers            _gpuBuffers       = {};

    public:
        // =============
        // Constructors
        // =============

        /** @brief Creates a default uninitialized instance. */
        Renderer() = default;

        // ==========
        // Utilities
        // ==========

        void Initialize(SDL_Window& window) override;
        void Deinitialize() override;
        void PrepareFrameResources() override;
        void Update() override;
        void SaveScreenshot() const override;

    private:
        // ========
        // Helpers
        // ========

        /** @brief Gets the GPU texture cache.
         *
         * @return GPU texture cache.
         */
        TextureCache& GetTextures();

        /** @brief Gets the GPU mesh cache.
         *
         * @return GPU mesh cache.
         */
        MeshCache& GetMeshes();

        /** @brief Gets the offscreen render texture.
         *
         * @return Offscreen render texture.
         */
        void UpdateRenderTargets();

        /** @brief Gets the offscreen depth stencil texture.
         *
         * @return Offscreen depth stencil texture.
         */
        SDL_GPUTexture* GetDepthTexture();

        /** @brief Gets the active texture sampler according to user options.
         *
         * @return Active texture sampler.
         */
        SDL_GPUSampler& GetActiveSampler();

        void Draw3dScene() override;
        void DrawDither() override;
        void Draw2dScene() override;
        void DrawPostProcess() override;
        void DrawViewport() override;
        void DrawPowerMenu() override;

        /** @brief Allocates memory pools for draw batches and GPU buffers. */
        void InitializeGpuBuffers();

        /** @brief Updates texture and mesh resources on the GPU.
         *
         * @param copyPass GPU copy pass.
         */
        void UpdateResources(SDL_GPUCopyPass& copyPass);

        /** @brief Converts render buffer data to 2D triangle GPU buffer data and uploads it to the GPU.
         *
         * Processes immediate-mode 2D sprites, shapes, and glyphs.
         *
         * @param copyPass GPU copy pass.
         */
        void CopyImmediatePrimitives2d(SDL_GPUCopyPass& copyPass);

        /** @brief Converts render buffer data to 3D triangle GPU buffer data and uploads it to the GPU.
         *
         * Processes immediate-mode 3D primitives.
         *
         * @param copyPass GPU copy pass.
         */
        void CopyImmediatePrimitives3d(SDL_GPUCopyPass& copyPass);

        /** @brief Converts render buffer data to viewport quad GPU buffer data and uploads it to the GPU.
         *
         * Processes a fullscreen quad.
         *
         * @param copyPass GPU copy pass.
         */
        void CopyGpuViewportQuad(SDL_GPUCopyPass& copyPass);

        /** @brief Pushes uniform data to the GPU for the vertex shader.
         *
         * @param uni Uniform buffer to push.
         * @param slotIdx Vertex uniform shader slot to push data to.
         */
        void PushVertexUniform(const UniformType& uni, int slotIdx);

        /** @brief Pushes uniform data to the GPU for the fragment shader.
         *
         * @param uni Uniform buffer to push.
         * @param slotIdx Fragment uniform shader slot to push data to.
         */
        void PushFragmentUniform(const UniformType& uni, int slotIdx);

        /** @brief Clears draw batches for reuse. */
        void ClearDrawBatches();
    };
}
