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
        auto ReserveMemory = [](RendererFrame& frame)
        {
            frame.DebugGuiDrawCalls.reserve(DEBUG_GUI_COUNT_MAX);
            frame.ImmediatePrimitives2d.reserve(SHAPE_2D_COUNT_MAX  +
                                                SPRITE_2D_COUNT_MAX +
                                                GLYPH_2D_COUNT_MAX);
            frame.ImmediatePrimitives3d.reserve(TRI_3D_COUNT_MAX);
        };
        ReserveMemory(_scene.Frame.Back);
        ReserveMemory(_scene.Frame.Front);

        _scene.Objects.Shapes2d.reserve(SHAPE_2D_COUNT_MAX);
        _scene.Objects.Sprites2d.reserve(SPRITE_2D_COUNT_MAX);
        _scene.Objects.Glyphs2d.reserve(GLYPH_2D_COUNT_MAX);
        _scene.Objects.Triangles3d.reserve(TRI_3D_COUNT_MAX);
    }

    void RendererBase::ProcessShapes2d()
    {
        for (const auto& shape : _scene.Objects.Shapes2d)
        {
            auto ndcs = std::vector<Vector2>{};
            ndcs.reserve(shape.Vertices.size());

            auto pos = Vector2::Zero;
            for (const auto& vert : shape.Vertices)
            {
                pos += vert.Position;
            }
            pos /= shape.Vertices.size();

            auto ndc = ConvertScreenPercentToNdc(pos);

            // Compute aspect correction.
            auto aspectCorrection = GetScreenAspectCorrection(shape.ScaleMd);

            // Triangle.
            if (shape.Vertices.size() == TRI_VERTEX_COUNT)
            {
                // Compute vertex positions.
                ndcs =
                {
                    ConvertScreenPercentToNdc(shape.Vertices[0].Position),
                    ConvertScreenPercentToNdc(shape.Vertices[1].Position),
                    ConvertScreenPercentToNdc(shape.Vertices[2].Position)
                };
            }
            // Line or quad.
            else if (shape.Vertices.size() == QUAD_VERTEX_COUNT)
            {
                // Compute vertex positions.
                ndcs =
                {
                    ConvertScreenPercentToNdc(shape.Vertices[0].Position),
                    ConvertScreenPercentToNdc(shape.Vertices[1].Position),
                    ConvertScreenPercentToNdc(shape.Vertices[2].Position),
                    ConvertScreenPercentToNdc(shape.Vertices[3].Position)
                };
            }

            // Create vertices.
            auto verts = std::vector<Vertex2d>{};
            verts.reserve(shape.Vertices.size());
            for (int i = 0; i < shape.Vertices.size(); i++)
            {
                verts.push_back(Vertex2d
                {
                    .Position = ndcs[i],
                    .Col      = shape.Vertices[i].Col
                });
            }

            // Add 2D primitive.
            // @lock Restrict 2D primitives access.
            {
                auto lock = ParallelLock(_scene.Primitives2dMutex);

                _scene.Frame.Back.ImmediatePrimitives2d.push_back(Primitive2d
                {
                    .Vertices    = std::move(verts),
                    .Depth       = shape.Depth,
                    .TextureName = {},
                    .RenderStg   = RenderStage::Shape2d,
                    .BlendMd     = shape.BlendMd,
                    .Uniform     = UniformSprite2dPerObject
                    {
                        .IsFastAlpha = shape.BlendMd == BlendMode::FastAlpha
                    }
                });
            }
        }

        _scene.Objects.Shapes2d.clear();
    }

    void RendererBase::ProcessSprites2d()
    {
        for (const auto& sprite : _scene.Objects.Sprites2d)
        {
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
            auto ndc  = ConvertScreenPercentToNdc(sprite.Position);
            auto pos0 = ndc + relPos0;
            auto pos1 = ndc + relPos1;
            auto pos2 = ndc + relPos2;
            auto pos3 = ndc + relPos3;

            // Compute vertex UVs.
            auto uv0 = sprite.UvMin;
            auto uv1 = Vector2(sprite.UvMax.x, sprite.UvMin.y);
            auto uv2 = sprite.UvMax;
            auto uv3 = Vector2(sprite.UvMin.x, sprite.UvMax.y);

            // Add 2D primitive.
            // @lock Restrict 2D primitives access.
            {
                auto lock = ParallelLock(_scene.Primitives2dMutex);

                _scene.Frame.Back.ImmediatePrimitives2d.push_back(Primitive2d
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
                    .Uniform     = UniformSprite2dPerObject
                    {
                        .IsFastAlpha = sprite.BlendMd == BlendMode::FastAlpha
                    }
                });
            }
        }

        _scene.Objects.Sprites2d.clear();
    }

    void RendererBase::ProcessGlyphs2d()
    {
        for (const auto& glyph : _scene.Objects.Glyphs2d)
        {
            // Set alignment offset.
            auto offset = Vector2(glyph.Scale.x, glyph.Scale.y);

            // Compute aspect correction.
            auto aspectCorrection = GetScreenAspectCorrection(GLYPH_SCALE_MODE);

            // Compute relative vertex positions.
            auto rotMat  = Matrix::CreateRotationZ(-glyph.Rotation);
            auto relPos0 = Vector2::Transform(Vector2(-glyph.Scale.x, glyph.Scale.y) + offset, rotMat) * aspectCorrection;
            auto relPos1 = Vector2::Transform(glyph.Scale                            + offset, rotMat) * aspectCorrection;
            auto relPos2 = Vector2::Transform(Vector2(glyph.Scale.x, -glyph.Scale.y) + offset, rotMat) * aspectCorrection;
            auto relPos3 = Vector2::Transform(-glyph.Scale                           + offset, rotMat) * aspectCorrection;

            // Compute vertex positions.
            auto ndc  = ConvertScreenPercentToNdc(glyph.Position);
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
                auto lock = ParallelLock(_scene.Primitives2dMutex);

                _scene.Frame.Back.ImmediatePrimitives2d.push_back(Primitive2d
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
                    .Uniform     = UniformGlyph2dPerObject
                    {
                        .HasGradient    = glyph.HasGradient,
                        .GradientUvMinY = glyph.GradientUvMinY,
                        .GradientUvMaxY = glyph.GradientUvMaxY
                    }
                });
            }
        }

        _scene.Objects.Glyphs2d.clear();
    }

    void RendererBase::ProcessTriangles3d()
    {
        for (const auto& tri : _scene.Objects.Triangles3d)
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
                auto lock = ParallelLock(_scene.Primitives3dMutex);

                _scene.Frame.Back.ImmediatePrimitives3d.push_back(Primitive3d
                {
                    .Vertices    = std::move(verts),
                    .TextureName = tri.TextureName,
                    .RenderStg   = RenderStage::Model,
                    .BlendMd     = tri.BlendMd,
                    .Uniform     = UniformModelPerObject
                    {
                        .IsFastAlpha = tri.BlendMd == BlendMode::FastAlpha
                    }
                });
            }
        }

        _scene.Objects.Triangles3d.clear();
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
                Sort(_scene.Frame.Front.ImmediatePrimitives2d, [](const Primitive2d& prim0, const Primitive2d& prim1)
                {
                    return prim0.Depth > prim1.Depth;
                });
            },

            // Sort 3D primitives.
            [&]()
            {
                Sort(_scene.Frame.Front.ImmediatePrimitives3d, [](const Primitive3d& prim0, const Primitive3d& prim1)
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
