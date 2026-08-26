#include "Framework.h"
#include "Renderer/Renderer.h"

#include "Application.h"
#include "Renderer/Common/Utils.h"
#include "Utils/Parallel.h"
#include "Utils/Utils.h"

using namespace Silent::Utils;

namespace Silent::Renderer
{
    void RendererBase::InitializeDoubleBuffer()
    {
        auto ReserveMemory = [](SceneDoubleBuffer::Data& data)
        {
            data.DebugGuiDrawCalls.reserve(DEBUG_GUI_COUNT_MAX);
            data.ImmediatePrimitives2d.reserve(SHAPE_2D_COUNT_MAX + 
                                               SPRITE_2D_COUNT_MAX + 
                                               GLYPH_2D_COUNT_MAX);
            data.ImmediatePrimitives3d.reserve(TRI_3D_COUNT_MAX);
        };
        ReserveMemory(_sceneBuffer.Active);
        ReserveMemory(_sceneBuffer.Render);

        _sceneObjects.Shapes2d.reserve(SHAPE_2D_COUNT_MAX);
        _sceneObjects.Sprites2d.reserve(SPRITE_2D_COUNT_MAX);
        _sceneObjects.Glyphs2d.reserve(GLYPH_2D_COUNT_MAX);
        _sceneObjects.Triangles3d.reserve(TRI_3D_COUNT_MAX);
    }

    void RendererBase::ProcessShapes2d()
    {
        // @todo How to apply this?
        // Compute aspect correction.
        auto aspectCorrection = GetScreenAspectCorrection(ScaleMode::VerticalEdge);

        for (const auto& shape : _sceneObjects.Shapes2d)
        {
            auto posArr = std::vector<Vector2>{};
            posArr.reserve(shape.Vertices.size());

            // Triangle.
            if (shape.Vertices.size() == TRI_VERTEX_COUNT)
            {
                // Compute vertex positions.
                posArr =
                {
                    ConvertScreenPercentToNdc(Vector2(shape.Vertices[0].Position.x, shape.Vertices[0].Position.y)),
                    ConvertScreenPercentToNdc(Vector2(shape.Vertices[1].Position.x, shape.Vertices[1].Position.y)),
                    ConvertScreenPercentToNdc(Vector2(shape.Vertices[2].Position.x, shape.Vertices[2].Position.y))
                };
            }
            // Line or quad.
            else if (shape.Vertices.size() == QUAD_VERTEX_COUNT)
            {
                // Compute vertex positions.
                posArr =
                {
                    ConvertScreenPercentToNdc(Vector2(shape.Vertices[0].Position.x, shape.Vertices[0].Position.y)),
                    ConvertScreenPercentToNdc(Vector2(shape.Vertices[1].Position.x, shape.Vertices[1].Position.y)),
                    ConvertScreenPercentToNdc(Vector2(shape.Vertices[2].Position.x, shape.Vertices[2].Position.y)),
                    ConvertScreenPercentToNdc(Vector2(shape.Vertices[3].Position.x, shape.Vertices[3].Position.y))
                };
            }

            // Create vertices.
            auto verts = std::vector<Vertex2d>{};
            verts.reserve(shape.Vertices.size());
            for (int i = 0; i < shape.Vertices.size(); i++)
            {
                verts.push_back(Vertex2d
                {
                    .Position = posArr[i],
                    .Col      = shape.Vertices[i].Col
                });
            }

            // Add 2D primitive.
            // @lock Restrict 2D primitives access.
            {
                auto lock = ParallelLock(_sceneBuffer.Primitives2dMutex);

                _sceneBuffer.Active.ImmediatePrimitives2d.push_back(Primitive2d
                {
                    .Vertices    = std::move(verts),
                    .Depth       = shape.Depth,
                    .TextureName = {},
                    .RenderStg   = RenderStage::Shape2d,
                    .BlendMd     = shape.BlendMd,
                    .Uniform     = UniformSprite2d
                    {
                        .IsFastAlpha = shape.BlendMd == BlendMode::FastAlpha
                    }
                });
            }
        }

        _sceneObjects.Shapes2d.clear();
    }

    void RendererBase::ProcessSprites2d()
    {
        for (const auto& sprite : _sceneObjects.Sprites2d)
        {
            // @todo Apply scale mode later.
            //auto pos = GetAspectCorrectScreenPosition(Vector2(vert.Position.x, vert.Position.y), sprite.ScaleMd);
            auto ndc = ConvertScreenPercentToNdc(sprite.Position);

            // Set alignment offset.
            auto offset = Vector2::Zero;
            switch (sprite.AlignMd)
            {
                default:
                case AlignMode::Center:
                {
                    break;
                }
                case AlignMode::CenterTop:
                {
                    offset = Vector2(0.0f, -sprite.Scale.y);
                    break;
                }
                case AlignMode::CenterBottom:
                {
                    offset = Vector2(0.0f, sprite.Scale.y);
                    break;
                }
                case AlignMode::CenterLeft:
                {
                    offset = Vector2(sprite.Scale.x, 0.0f);
                    break;
                }
                case AlignMode::CenterRight:
                {
                    offset = Vector2(-sprite.Scale.x, 0.0f);
                    break;
                }
                case AlignMode::TopLeft:
                {
                    offset = Vector2(sprite.Scale.x, -sprite.Scale.y);
                    break;
                }
                case AlignMode::TopRight:
                {
                    offset = Vector2(-sprite.Scale.x, -sprite.Scale.y);
                    break;
                }
                case AlignMode::BottomLeft:
                {
                    offset = Vector2(sprite.Scale.x, sprite.Scale.y);
                    break;
                }
                case AlignMode::BottomRight:
                {
                    offset = Vector2(-sprite.Scale.x, sprite.Scale.y);
                    break;
                }
            }

            // Compute aspect correction.
            auto aspectCorrection = GetScreenAspectCorrection(sprite.ScaleMd);

            // Compute relative vertex positions.
            auto rotMat  = Matrix::CreateRotationZ(-sprite.Rotation);
            auto relPos0 = Vector2::Transform(Vector2(-sprite.Scale.x, sprite.Scale.y) + offset, rotMat) * aspectCorrection;
            auto relPos1 = Vector2::Transform(sprite.Scale                             + offset, rotMat) * aspectCorrection;
            auto relPos2 = Vector2::Transform(Vector2(sprite.Scale.x, -sprite.Scale.y) + offset, rotMat) * aspectCorrection;
            auto relPos3 = Vector2::Transform(-sprite.Scale                            + offset, rotMat) * aspectCorrection;

            // Compute vertex positions.
            auto pos0 = Vector2(ndc.x + relPos0.x, ndc.y + relPos0.y);
            auto pos1 = Vector2(ndc.x + relPos1.x, ndc.y + relPos1.y);
            auto pos2 = Vector2(ndc.x + relPos2.x, ndc.y + relPos2.y);
            auto pos3 = Vector2(ndc.x + relPos3.x, ndc.y + relPos3.y);

            // Compute vertex UVs.
            auto uv0 = sprite.UvMin;
            auto uv1 = Vector2(sprite.UvMax.x, sprite.UvMin.y);
            auto uv2 = sprite.UvMax;
            auto uv3 = Vector2(sprite.UvMin.x, sprite.UvMax.y);

            // Add 2D primitive.
            // @lock Restrict 2D primitives access.
            {
                auto lock = ParallelLock(_sceneBuffer.Primitives2dMutex);

                _sceneBuffer.Active.ImmediatePrimitives2d.push_back(Primitive2d
                {
                    .Vertices =
                    {
                        Vertex2d
                        {
                            .Position   = pos0,
                            .Uv         = uv0,
                            .Col        = sprite.Col0,
                            .PaletteIdx = sprite.PaletteIdx
                        },
                        Vertex2d
                        {
                            .Position   = pos1,
                            .Uv         = uv1,
                            .Col        = sprite.Col1,
                            .PaletteIdx = sprite.PaletteIdx
                        },
                        Vertex2d
                        {
                            .Position   = pos2,
                            .Uv         = uv2,
                            .Col        = sprite.Col2,
                            .PaletteIdx = sprite.PaletteIdx
                        },
                        Vertex2d
                        {
                            .Position   = pos3,
                            .Uv         = uv3,
                            .Col        = sprite.Col3,
                            .PaletteIdx = sprite.PaletteIdx
                        }
                    },
                    .Depth       = sprite.Depth,
                    .TextureName = sprite.TextureName,
                    .RenderStg   = RenderStage::Sprite2d,
                    .BlendMd     = sprite.BlendMd,
                    .Uniform     = UniformSprite2d
                    {
                        .IsFastAlpha = sprite.BlendMd == BlendMode::FastAlpha
                    }
                });
            }
        }

        _sceneObjects.Sprites2d.clear();
    }

    void RendererBase::ProcessGlyphs2d()
    {
        for (const auto& glyph : _sceneObjects.Glyphs2d)
        {
            //auto pos = GetAspectCorrectScreenPosition(Vector2(vert.Position.x, vert.Position.y), sprite.ScaleMd);
            auto ndc = ConvertScreenPercentToNdc(glyph.Position);

            // Set alignment offset and aspect correction.
            auto offset           = Vector2(glyph.Scale.x, glyph.Scale.y);
            auto aspectCorrection = GetScreenAspectCorrection(glyph.ScaleMd);

            // Compute relative vertex positions.
            auto rotMat  = Matrix::CreateRotationZ(-glyph.Rotation);
            auto relPos0 = Vector2::Transform(Vector2(-glyph.Scale.x, glyph.Scale.y) + offset, rotMat) * aspectCorrection;
            auto relPos1 = Vector2::Transform(glyph.Scale                            + offset, rotMat) * aspectCorrection;
            auto relPos2 = Vector2::Transform(Vector2(glyph.Scale.x, -glyph.Scale.y) + offset, rotMat) * aspectCorrection;
            auto relPos3 = Vector2::Transform(-glyph.Scale                           + offset, rotMat) * aspectCorrection;

            // Compute vertex positions.
            auto pos0 = ndc + relPos0;
            auto pos1 = ndc + relPos1;
            auto pos2 = ndc + relPos2;
            auto pos3 = ndc + relPos3;

            // Compute vertex UVs.
            auto uv0 = glyph.UvMin;
            auto uv1 = Vector2(glyph.UvMax.x, glyph.UvMin.y);
            auto uv2 = glyph.UvMax;
            auto uv3 = Vector2(glyph.UvMin.x, glyph.UvMax.y);

            // Add 2D primitive.
            // @lock Restrict 2D primitives access.
            {
                auto lock = ParallelLock(_sceneBuffer.Primitives2dMutex);

                _sceneBuffer.Active.ImmediatePrimitives2d.push_back(Primitive2d
                {
                    .Vertices =
                    {
                        Vertex2d
                        {
                            .Position = pos0,
                            .Uv       = uv0,
                            .Col      = glyph.Col
                        },
                        Vertex2d
                        {
                            .Position = pos1,
                            .Uv       = uv1,
                            .Col      = glyph.Col
                        },
                        Vertex2d
                        {
                            .Position = pos2,
                            .Uv       = uv2,
                            .Col      = glyph.Col
                        },
                        Vertex2d
                        {
                            .Position = pos3,
                            .Uv       = uv3,
                            .Col      = glyph.Col
                        }
                    },
                    .Depth       = glyph.Depth,
                    .TextureName = glyph.AtlasName,
                    .RenderStg   = RenderStage::Glyph2d,
                    .BlendMd     = BlendMode::Alpha,
                    .Uniform     = UniformGlyph2d
                    {
                        .HasGradient    = glyph.HasGradient,
                        .GradientSteps  = (uint)glyph.GradientSteps,
                        .GradientUvMinY = glyph.GradientUvMinY,
                        .GradientUvMaxY = glyph.GradientUvMaxY
                    }
                });
            }
        }

        _sceneObjects.Glyphs2d.clear();
    }

    void RendererBase::ProcessTriangles3d()
    {
        for (const auto& tri : _sceneObjects.Triangles3d)
        {
            auto verts = std::vector<Vertex3d>{};
            verts.reserve(tri.Vertices.size());
            for (const auto& vert : tri.Vertices)
            {
                verts.push_back(Vertex3d
                {
                    .Position   = vert.Position,
                    .Normal     = vert.Normal,
                    .Uv         = vert.Uv,
                    .Col        = vert.Col,
                    .PaletteIdx = vert.PaletteIdx
                });
            }

            // Add 3D primitive.
            // @lock Restrict 3D primitives access.
            {
                auto lock = ParallelLock(_sceneBuffer.Primitives3dMutex);

                _sceneBuffer.Active.ImmediatePrimitives3d.push_back(Primitive3d
                {
                    .Vertices    = std::move(verts),
                    .TextureName = tri.TextureName,
                    .RenderStg   = RenderStage::Model,
                    .BlendMd     = tri.BlendMd,
                    .Uniform     = UniformModel
                    {
                        .IsFastAlpha = tri.BlendMd == BlendMode::FastAlpha
                    }
                });
            }
        }

        _sceneObjects.Triangles3d.clear();
    }

    void RendererBase::SortRenderBufferData()
    {
        auto& executor = g_App.GetExecutor();

        // @todo Use sort keys?
        auto sortTasks = ParallelTasks
        {
            // Sort 2D primitives.
            [&]()
            {
                Sort(_sceneBuffer.Render.ImmediatePrimitives2d, [](const Primitive2d& prim0, const Primitive2d& prim1)
                {
                    return prim0.Depth > prim1.Depth;
                });
            },

            // Sort 3D primitives.
            [&]()
            {
                Sort(_sceneBuffer.Render.ImmediatePrimitives3d, [](const Primitive3d& prim0, const Primitive3d& prim1)
                {
                    return true;
                });
            }
        };
        executor.AddTasks(sortTasks).wait();
    }

    void RendererBase::DrawFrame()
    {
        // Scene.
        Draw3dScene();
        Draw3dScenePostProcess();
        Draw2dScene();

        // Final image.
        DrawScenePostProcess();
        DrawViewport();

        // @debug
        if (Debug::g_Work.EnableDebugMenu)
        {
            DrawDebugMenu();
        }
    }
}
