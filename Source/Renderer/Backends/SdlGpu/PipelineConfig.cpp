#include "Framework.h"
#include "Renderer/Backends/SdlGpu/PipelineConfig.h"

#include "Renderer/Backends/SdlGpu/Gpu/Layouts/BufferVertex2d.h"
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
        // 2D sprite.
        PipelineConfig
        {
            .Stage                        = RenderStage::Sprite2d,
            .VertexShaderName             = "Primitive2d.vert",
            .VertShaderSamplerCount       = 0,
            .VertShaderUniBufferCount     = 0,
            .VertShaderStorageBufferCount = 0,
            .VertShaderStorageTexCount    = 0,
            .FragmentShaderName           = "Sprite2d.frag",
            .FragShaderSamplerCount       = 1,
            .FragShaderUniBufferCount     = 1,
            .FragShaderStorageBufferCount = 0,
            .FragShaderStorageTexCount    = 0,
            .VertBufferDescs              =
            {
                {
                    .slot               = 0,
                    .pitch              = sizeof(BufferVertex2d),
                    .input_rate         = SDL_GPU_VERTEXINPUTRATE_VERTEX,
                    .instance_step_rate = 0
                }
            },
            .VertBufferAttribs =
            {
                // `BufferVertex2d::Position`
                {
                    .location    = 0,
                    .buffer_slot = 0,
                    .format      = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
                    .offset      = 0
                },
                // `BufferVertex2d::Uv`
                {
                    .location    = 1,
                    .buffer_slot = 0,
                    .format      = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
                    .offset      = sizeof(Vector3)
                },
                // `BufferVertex2d::Col`
                {
                    .location    = 2,
                    .buffer_slot = 0,
                    .format      = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
                    .offset      = sizeof(Vector3) +
                                   sizeof(Vector2)
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
        },
        // 2D glyph.
        PipelineConfig
        {
            .Stage                        = RenderStage::Glyph2d,
            .VertexShaderName             = "Primitive2d.vert",
            .VertShaderSamplerCount       = 0,
            .VertShaderUniBufferCount     = 0,
            .VertShaderStorageBufferCount = 0,
            .VertShaderStorageTexCount    = 0,
            .FragmentShaderName           = "Glyph2d.frag",
            .FragShaderSamplerCount       = 1,
            .FragShaderUniBufferCount     = 1,
            .FragShaderStorageBufferCount = 0,
            .FragShaderStorageTexCount    = 0,
            .VertBufferDescs              =
            {
                {
                    .slot               = 0,
                    .pitch              = sizeof(BufferVertex2d),
                    .input_rate         = SDL_GPU_VERTEXINPUTRATE_VERTEX,
                    .instance_step_rate = 0
                }
            },
            .VertBufferAttribs =
            {
                // `BufferVertex2d::Position`
                {
                    .location    = 0,
                    .buffer_slot = 0,
                    .format      = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
                    .offset      = 0
                },
                // `BufferVertex2d::Uv`
                {
                    .location    = 1,
                    .buffer_slot = 0,
                    .format      = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
                    .offset      = sizeof(Vector3)
                },
                // `BufferVertex2d::Col`
                {
                    .location    = 2,
                    .buffer_slot = 0,
                    .format      = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
                    .offset      = sizeof(Vector3) +
                                   sizeof(Vector2)
                }
            },
            .BlendModes =
            {
                BlendMode::Alpha,
                BlendMode::Wireframe
            }
        }
    };
}
