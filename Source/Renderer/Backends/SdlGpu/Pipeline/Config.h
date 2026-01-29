#pragma once

#include "Renderer/Common/Enums.h"

namespace Silent::Renderer::SdlGpu
{
    /** @brief Pipeline configuration data. */
    struct PipelineConfig
    {
        RenderStage Stage = RenderStage::Sprite2d;

        std::string VertShaderName               = {};
        int         VertShaderSamplerCount       = 0;
        int         VertShaderUniBufferCount     = 0;
        int         VertShaderStorageBufferCount = 0;
        int         VertShaderStorageTexCount    = 0;

        std::string FragShaderName               = {};
        int         FragShaderSamplerCount       = 0;
        int         FragShaderUniBufferCount     = 0;
        int         FragShaderStorageBufferCount = 0;
        int         FragShaderStorageTexCount    = 0;

        std::vector<SDL_GPUVertexBufferDescription> VertBufferDescs   = {};
        std::vector<SDL_GPUVertexAttribute>         VertBufferAttribs = {};
        std::vector<SDL_GPUColorTargetDescription>  ColorTargetDescs  = {}; // @todo Maybe not needed except for special cases. Filled automatically.
        std::vector<BlendMode>                      BlendModes        = {};
    };

    extern const std::vector<SDL_GPUColorTargetBlendState> PIPELINE_BLEND_MODE_COLOR_TARGETS;
    extern const std::vector<PipelineConfig>               PIPELINE_CONFIGS;
}
