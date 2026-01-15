#include "Framework.h"
#include "Renderer/Backends/SdlGpu/PipelineConfig.h"

#include "Renderer/Backends/SdlGpu/Gpu/Layouts/Vertex2dBuffer.h"
#include "Renderer/Backends/SdlGpu/Pipeline.h"

namespace Silent::Renderer
{
    const std::vector<SDL_GPUColorTargetBlendState> PIPELINE_BLEND_MODE_COLOR_TARGETS = 
    {
        // Opaque.
        SDL_GPUColorTargetBlendState
        {
            .src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
            .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ZERO,
            .color_blend_op        = SDL_GPU_BLENDOP_ADD,
            .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
            .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO,
            .alpha_blend_op        = SDL_GPU_BLENDOP_ADD,
            .enable_blend          = true
        },
        // Alpha.
        SDL_GPUColorTargetBlendState
        {
            .src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
            .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
            .color_blend_op        = SDL_GPU_BLENDOP_ADD,
            .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
            .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
            .alpha_blend_op        = SDL_GPU_BLENDOP_ADD,
            .enable_blend          = true
        },
        // Fast alpha. Requires `IsFastAlpha` shader uniform set to `true`.
        SDL_GPUColorTargetBlendState
        {
            .src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
            .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
            .color_blend_op        = SDL_GPU_BLENDOP_ADD,
            .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
            .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
            .alpha_blend_op        = SDL_GPU_BLENDOP_ADD,
            .enable_blend          = true
        },
        // Multiply.
        SDL_GPUColorTargetBlendState
        {
            .src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_COLOR,
            .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
            .color_blend_op        = SDL_GPU_BLENDOP_ADD,
            .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
            .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
            .alpha_blend_op        = SDL_GPU_BLENDOP_ADD,
            .enable_blend          = true
        },
        // Add.
        SDL_GPUColorTargetBlendState
        {
            .src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
            .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
            .color_blend_op        = SDL_GPU_BLENDOP_ADD,
            .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
            .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
            .alpha_blend_op        = SDL_GPU_BLENDOP_ADD,
            .enable_blend          = true
        },
        // Subtract.
        SDL_GPUColorTargetBlendState
        {
            .src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
            .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
            .color_blend_op        = SDL_GPU_BLENDOP_SUBTRACT,
            .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
            .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
            .alpha_blend_op        = SDL_GPU_BLENDOP_SUBTRACT,
            .enable_blend          = true
        },
        // Wireframe.
        SDL_GPUColorTargetBlendState
        {
            .src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
            .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ZERO,
            .color_blend_op        = SDL_GPU_BLENDOP_ADD,
            .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
            .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO,
            .alpha_blend_op        = SDL_GPU_BLENDOP_ADD,
            .enable_blend          = true
        }
    };

    const std::vector<PipelineConfig> PIPELINE_CONFIGS =
    {
        // 2D triangle.
        PipelineConfig
        {
            .Stage                        = RenderStage::Triangle2d,
            .VertexShaderName             = "Triangle2d.vert",
            .VertShaderSamplerCount       = 0,
            .VertShaderUniBufferCount     = 0,
            .VertShaderStorageBufferCount = 0,
            .VertShaderStorageTexCount    = 0,
            .FragmentShaderName           = "Triangle2d.frag",
            .FragShaderSamplerCount       = 1,
            .FragShaderUniBufferCount     = 1,
            .FragShaderStorageBufferCount = 0,
            .FragShaderStorageTexCount    = 0,
            .VertBufferDescs              =
            {
                {
                    .slot               = 0,
                    .pitch              = sizeof(Vertex2dBuffer),
                    .input_rate         = SDL_GPU_VERTEXINPUTRATE_VERTEX,
                    .instance_step_rate = 0
                }
            },
            .VertBufferAttribs =
            {
                {
                    .location    = 0,
                    .buffer_slot = 0,
                    .format      = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
                    .offset      = 0
                },
                {
                    .location    = 1,
                    .buffer_slot = 0,
                    .format      = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
                    .offset      = sizeof(float) * Vector3::AXIS_COUNT
                },
                {
                    .location    = 2,
                    .buffer_slot = 0,
                    .format      = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
                    .offset      = (sizeof(float) * Vector3::AXIS_COUNT) +
                                   (sizeof(float) * Vector2::AXIS_COUNT)
                }
            },
            .BlendModes =
            {
                BlendMode::Opaque,
                BlendMode::Alpha,
                BlendMode::FastAlpha,
                BlendMode::Multiply,
                BlendMode::Add,
                BlendMode::Subtract,
                BlendMode::Wireframe
            }
        }
    };
}
