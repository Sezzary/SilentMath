#pragma once

namespace Silent::Renderer{ enum class BlendMode;   }
namespace Silent::Renderer{ enum class RenderStage; }

namespace Silent::Renderer::SdlGpu
{
    struct PipelineConfig;

    /** @brief GPU pipeline manager. */
    class PipelineManager
    {
    private:
        // =======
        // Fields
        // =======

        SDL_GPUDevice*                                    _device    = nullptr;
        std::unordered_map<int, SDL_GPUGraphicsPipeline*> _pipelines = {}; /** Key = pipeline hash, value = pipeline. */

    public:
        // =============
        // Constructors
        // =============

        /** @brief Constructs an uninitialized default `PipelineManager`. */
        PipelineManager() = default;

        // ==========
        // Utilities
        // ==========

        /** @brief Initializes all pipelines.
         *
         * @param window App window.
         * @param device GPU device.
         */
        void Initialize(SDL_Window& window, SDL_GPUDevice& device);

        /** @brief Deinitializes all pipelines and frees GPU resources. */
        void Deinitialize();

        /** @brief Binds the graphics pipeline render stage for use in rendering.
         *
         * @param renderPass Render pass to bind the pipeline to.
         * @param renderStage Pipeline render stage to bind.
         * @throws `std::runtime_error` if the pipeline is invalid.
         */
        void Bind(SDL_GPURenderPass& renderPass, RenderStage renderStage, BlendMode blendMode);

    private:
        // ========
        // Helpers
        // ========

        /** @brief Initializes the graphics pipeline with vertex and fragment shaders.
         *
         * @param window App window.
         * @param config Pipeline configuration details.
         */
        void InitializeGraphicsPipeline(SDL_Window& window, const PipelineConfig& config);

        /** @brief Loads a vertex or fragment shader.
         *
         * @param filename Shader filename. Suffix must be `.vert` or `.frag`.
         * @param samplerCount Sampler count.
         * @param uniBufferCount Uniform buffer count.
         * @param storageBufferCount Storage buffer count.
         * @param storageTexCount Storage texture count.
         * @return Compiled vertex or fragment shader.
         */
        SDL_GPUShader* LoadShader(const std::string& filename, int samplerCount, int storageTexCount, int storageBufferCount, int uniBufferCount);

        /** @brief Computes a pipeline hash from a render stage and blend mode.
         *
         * @note This produces perfect hashing.
         *
         * @param renderStage Render stage.
         * @param blendMode Blend mode.
         * @return Pipeline hash.
         */
        int GetPipelineHash(RenderStage renderStage, BlendMode blendMode);
    };
}
