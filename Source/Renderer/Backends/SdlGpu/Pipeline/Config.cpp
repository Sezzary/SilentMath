#include "Framework.h"
#include "Renderer/Backends/SdlGpu/Pipeline/Config.h"

#include "Renderer/Common/Enums.h"
#include "Renderer/Common/Objects/Buffers.h"
#include "Renderer/Backends/SdlGpu/Pipeline/Pipeline.h"

namespace Silent::Renderer::SdlGpu
{
    static const auto BUFFER_VERTEX_2D_DESCS = std::vector<SDL_GPUVertexBufferDescription>
    {
        {
            .slot               = 0,
            .pitch              = sizeof(BufferVertex2d),
            .input_rate         = SDL_GPU_VERTEXINPUTRATE_VERTEX,
            .instance_step_rate = 0
        }
    };
    static const auto BUFFER_VERTEX_2D_ATTRIBS = std::vector<SDL_GPUVertexAttribute>
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
    };

    static const auto BUFFER_VERTEX_3D_DESCS = std::vector<SDL_GPUVertexBufferDescription>
    {
        {
            .slot               = 0,
            .pitch              = sizeof(BufferVertex3d),
            .input_rate         = SDL_GPU_VERTEXINPUTRATE_VERTEX,
            .instance_step_rate = 0
        }
    };
    static const auto BUFFER_VERTEX_3D_ATTRIBS = std::vector<SDL_GPUVertexAttribute>
    {
        // `BufferVertex3d::Position`
        {
            .location    = 0,
            .buffer_slot = 0,
            .format      = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
            .offset      = 0
        },
        // `BufferVertex3d::Normal`
        {
            .location    = 1,
            .buffer_slot = 0,
            .format      = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
            .offset      = sizeof(Vector3)
        },
        // `BufferVertex3d::Uv`
        {
            .location    = 2,
            .buffer_slot = 0,
            .format      = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
            .offset      = sizeof(Vector3) +
                           sizeof(Vector3)
        },
        // `BufferVertex3d::Col`
        {
            .location    = 3,
            .buffer_slot = 0,
            .format      = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
            .offset      = sizeof(Vector3) +
                           sizeof(Vector3) +
                           sizeof(Vector2)
        }
    };

    const std::vector<SDL_GPUColorTargetBlendState> PIPELINE_BLEND_MODE_COLOR_TARGETS = 
    {
        // Opaque.
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
        // =========
        // Material
        // =========

        // 2D sprite.
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
            .VertBufferDescs              = BUFFER_VERTEX_2D_DESCS,
            .VertBufferAttribs            = BUFFER_VERTEX_2D_ATTRIBS,
            .BlendModes                   =
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
            .VertBufferDescs              = BUFFER_VERTEX_2D_DESCS,
            .VertBufferAttribs            = BUFFER_VERTEX_2D_ATTRIBS,
            .BlendModes                   =
            {
                BlendMode::Alpha,
                BlendMode::Wireframe
            }
        },

        // =============
        // Post-process
        // =============

        // Fade.
        {
            .Stage                        = RenderStage::Fade,
            .VertexShaderName             = "Primitive2d.vert",
            .VertShaderSamplerCount       = 0,
            .VertShaderUniBufferCount     = 0,
            .VertShaderStorageBufferCount = 0,
            .VertShaderStorageTexCount    = 0,
            .FragmentShaderName           = "Fade.frag",
            .FragShaderSamplerCount       = 1,
            .FragShaderUniBufferCount     = 1,
            .FragShaderStorageBufferCount = 0,
            .FragShaderStorageTexCount    = 0,
            .VertBufferDescs              = BUFFER_VERTEX_2D_DESCS,
            .VertBufferAttribs            = BUFFER_VERTEX_2D_ATTRIBS,
            .BlendModes                   =
            {
                BlendMode::Alpha
            }
        },

        // ======
        // Final
        // ======

        // BLIT.
        {
            .Stage                        = RenderStage::Blit,
            .VertexShaderName             = "Primitive2d.vert",
            .VertShaderSamplerCount       = 0,
            .VertShaderUniBufferCount     = 0,
            .VertShaderStorageBufferCount = 0,
            .VertShaderStorageTexCount    = 0,
            .FragmentShaderName           = "Blit.frag",
            .FragShaderSamplerCount       = 1,
            .FragShaderUniBufferCount     = 1,
            .FragShaderStorageBufferCount = 0,
            .FragShaderStorageTexCount    = 0,
            .VertBufferDescs              = BUFFER_VERTEX_2D_DESCS,
            .VertBufferAttribs            = BUFFER_VERTEX_2D_ATTRIBS,
            .BlendModes                   =
            {
                BlendMode::Opaque
            }
        }
    };
}
