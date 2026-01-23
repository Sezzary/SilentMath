#pragma once

#include "Renderer/Backends/SdlGpu/Gpu/Buffer.h"
#include "Renderer/Backends/SdlGpu/Gpu/Layouts/BufferVertex2d.h"
#include "Renderer/Backends/SdlGpu/Gpu/Layouts/UniformGlyph2d.h"
#include "Renderer/Backends/SdlGpu/Gpu/Layouts/UniformSprite2d.h"
#include "Renderer/Backends/SdlGpu/Gpu/VertexBuffer.h"
#include "Renderer/Backends/SdlGpu/Pipeline.h"
#include "Renderer/Backends/SdlGpu/Texture.h"
#include "Renderer/Common/Objects/Primitive/Vertex2d.h"
#include "Renderer/Common/Objects/Primitive/Vertex3d.h"
#include "Renderer/Renderer.h"

namespace Silent::Renderer
{
    using UniformType = std::variant<UniformGlyph2d,
                                     UniformSprite2d>;

    /** @brief GPU buffer draw raw batch. */
    struct DrawBatch
    {
        std::string TextureName  = {};
        RenderStage RenderStg    = RenderStage::Sprite2d;
        BlendMode   BlendMd      = BlendMode::Opaque;
        UniformType Uniform      = {};
        int         BufferOffset = 0;
        int         BufferStride = 0;
    };

    /** @brief Sorted GPU buffer draw batches. */
    struct DrawBatches
    {
        std::vector<DrawBatch> Primitives2d = {}; // @todo Should be "Primitives2d".
    };

    /** @brief GPU buffers. */
    struct GpuBuffers
    {
        VertexBuffer<BufferVertex2d> Vertices2d = {};
    };

    /** @brief SDL_gpu renderer backend. */
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
        void InitializeMemory();

        /** @brief Adds new glyph texture atlases and updates old ones if new glyphs have been added.
         *
         * @todo Race condition! Should run this before render thread.
         *
         * @param copyPass Copy pass.
         */
        void UpdateFontAtlasTextures(SDL_GPUCopyPass& copyPass);

        /** @brief Converts render buffer data to 2D triangle GPU buffer data and uploads it to the GPU.
         *
         * Processes 2D sprites and shapes.
         *
         * @param copyPass Copy pass.
         */
        void CopyGpuPrimitives2d(SDL_GPUCopyPass& copyPass);

        /** @brief Pushes uniform data to the GPU for the vertex shader.
         *
         * @param uni Uniform buffer to push.
         * @param slotIdx Index of the vertex uniform slot to push data to.
         */
        void PushVertexUniform(const UniformType& uni, int slotIdx);

        /** @brief Pushes uniform data to the GPU for the fragment shader.
         *
         * @param uni Uniform buffer to push.
         * @param slotIdx Index of the fragment uniform slot to push data to.
         */
        void PushFragmentUniform(const UniformType& uni, int slotIdx);

        /** @brief Clears draw batches for reuse. */
        void ClearDrawBatches();
    };
}
