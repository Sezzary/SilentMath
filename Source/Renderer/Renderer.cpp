#include "Framework.h"
#include "Renderer/Renderer.h"

#include "Application.h"
#include "Renderer/Backends/SdlGpu/SdlGpu.h"
#include "Renderer/Common/Resources/Primitive/Primitive3d.h"
#include "Renderer/Common/Resources/Primitive/Vertex2d.h"
#include "Renderer/Common/Resources/Primitive/Vertex3d.h"
#include "Renderer/Common/Resources/Scene/Shape2d.h"
#include "Renderer/Common/Resources/Scene/Sprite2d.h"
#include "Renderer/Common/Resources/Scene/Text2d.h"
#include "Renderer/Common/Utils.h"
#include "Utils/Parallel.h"
#include "Utils/Utils.h"

using namespace Silent::Utils;

namespace Silent::Renderer
{
    RendererType RendererBase::GetType() const
    {
        return _type;
    }

    int RendererBase::GetDrawCallCount() const
    {
        return _doubleBuffer.Render.DrawCallCount;
    }

    void RendererBase::SetClearColor(const Color& color)
    {
        _clearColor = color;
    }

    Vector2i RendererBase::GetScreenResolution() const
    {
        const auto& options = g_App.GetOptions();

        auto res = g_App.GetWindowResolution();
        return res;

        // @todo Render scale should be a post-process instead?
        switch (options->RenderScale)
        {
            case RenderScaleType::Native:
            {
                break;
            }
            case RenderScaleType::Retro:
            {
                res = RETRO_SCREEN_SPACE_RES.ToVector2i();
                break;
            }
        }

        return res;
    }

    float RendererBase::GetScreenAspectRatio() const
    {
        auto res = GetScreenResolution().ToVector2();
        return res.x / res.y;
    }

    void RendererBase::PrepareRenderBuffer()
    {
        auto& executor = g_App.GetExecutor();

        // @todo Need to call `UpdateFontAtlasTextures` here. Backends need their own
        // pre-render data prep method.

        // @todo Using parallelism causes flickering here. Why?
        // Generate active buffer data.
        //auto tasks = ParallelTasks
        //{
        //    TASK(ProcessShapes2d()),
        //    TASK(ProcessSprites2d()),
        //    TASK(ProcessGlyphs2d())
        //};
        //executor.AddTasks(tasks).wait();
        ProcessShapes2d();
        ProcessSprites2d();
        ProcessGlyphs2d();

        // Swap double buffer.
        std::swap(_doubleBuffer.Render, _doubleBuffer.Active);

        // Clear active buffer.
        _doubleBuffer.Active.DrawCallCount = 0;
        _doubleBuffer.Active.Primitives2d.clear();
        _doubleBuffer.Active.Primitives3d.clear();
        _doubleBuffer.Active.DebugPrimitives3d.clear();
        _doubleBuffer.Active.DebugGuiDrawCalls.clear();

        _shapes2d.clear();
        _sprites2d.clear();
        _glyphs2d.clear();
    }

    void RendererBase::SignalResize()
    {
        _isResized = true;
    }

    bool RendererBase::SubmitShape2d(const Shape2d& shape)
    {
        if (_shapes2d.size() >= SHAPE_2D_COUNT_MAX)
        {
            Debug::Log("Attempted to submit 2D shape to full container.", Debug::LogLevel::Warning, Debug::LogMode::Debug);
            return false;
        }

        _shapes2d.push_back(shape);
        return true;
    }

    bool RendererBase::SubmitSprite2d(const Sprite2d& sprite)
    {
        auto& assets = g_App.GetAssets();

        if (_sprites2d.size() >= SPRITE_2D_COUNT_MAX)
        {
            Debug::Log("Attempted to submit 2D sprite to full container.", Debug::LogLevel::Warning, Debug::LogMode::Debug);
            return false;
        }

        // @todo Handle this somehow somewhere, but not here. Sprite textures don't
        // have to come from streamable assets, e.g. font atlases.
        //const auto asset = assets.GetAsset(sprite.TextureName);
        //if (asset->Type != AssetType::Tim)
        //{
        //    Debug::Log("Attempted to submit non-image asset as screen sprite.", Debug::LogLevel::Warning, Debug::LogMode::Debug);
        //    return false;
        //}

        _sprites2d.push_back(sprite);
        return true;
    }

    bool RendererBase::SubmitText2d(const Text2d& text)
    {
        constexpr auto SHADOW_COLOR  = Color::From8Bit(16, 16, 16);
        static const auto SHADOW_OFFSET = SCREEN_SPACE_RES * (1.0f / RETRO_SCREEN_SPACE_RES.y); // @todo Make `constexpr`.

        auto& fonts = g_App.GetFonts();

        // Get font.
        auto* font = fonts.GetFont(text.FontName);
        if (font == nullptr)
        {
            Debug::Log(Fmt("Attempted to submit 2D text with missing font `{}`.", text.FontName), Debug::LogLevel::Warning, Debug::LogMode::Debug);
            return false;
        }

        // Get shaped text glyphs.
        auto shapedText = font->GetShapedText(text.Message);

        // Compute trasformation parameters.
        auto rotMat          = Matrix::CreateRotationZ(text.Rotation);
        auto fontScaleFactor = SCREEN_SPACE_RES / (float)font->GetPointSize();
        auto textSize        = (Vector2(shapedText.Width, (float)font->GetPointSize()) * fontScaleFactor) * text.Scale;

        // @todo Derive colour from markup.
        auto color = Color::White;
        //color.A()  = text.Opacity;

        // Compute text position. @todo Alignment should be in markup.
        // @todo Use common function for alignment pivots.
        auto textOffset = Vector2::Zero;
        switch (text.AlignMd)
        {
            case AlignMode::Center:
            {
                textOffset = Vector2(-textSize.x, textSize.y) * 0.5f;
                break;
            }
            case AlignMode::CenterTop:
            {
                textOffset = Vector2(-textSize.x * 0.5f, textSize.y);
                break;
            }
            case AlignMode::CenterBottom:
            {
                textOffset = Vector2(-textSize.x * 0.5f, 0.0f);
                break;
            }
            case AlignMode::CenterLeft:
            {
                textOffset = Vector2(0.0f, textSize.y * 0.5f);
                break;
            }
            case AlignMode::CenterRight:
            {
                textOffset = Vector2(-textSize.x, textSize.y * 0.5f);
                break;
            }
            case AlignMode::TopLeft:
            {
                textOffset = Vector2(0.0f, textSize.y);
                break;
            }
            case AlignMode::TopRight:
            {
                textOffset = Vector2(-textSize.x, textSize.y);
                break;
            }
            default:
            case AlignMode::BottomLeft:
            {
                break;
            }
            case AlignMode::BottomRight:
            {
                textOffset = Vector2(-textSize.x, 0.0f);
                break;
            }
        }
        auto adjTextPos = text.Position + Vector2::Transform(textOffset, rotMat);

        // Run through shaped glyphs.
        auto pixelOffset = Vector2::Zero;
        for (const auto& shapedGlyph : shapedText.Glyphs)
        {
            // Compute texture atlas UVs.
            auto uvMin = shapedGlyph.Attribs.AtlasPosition.ToVector2() / Vector2(Font::ATLAS_SIZE); 
            auto uvMax = uvMin + (shapedGlyph.Attribs.AtlasSize.ToVector2() / Vector2(Font::ATLAS_SIZE));

            // Compute rotated offset.
            auto adjPixelOffset = Vector2::Transform(pixelOffset, rotMat);

            // Compute rotated bearing.
            auto pixelBearing    = Vector2(shapedGlyph.Attribs.Bearing.x, shapedGlyph.Attribs.AtlasSize.y - shapedGlyph.Attribs.Bearing.y);
            auto adjPixelBearing = Vector2::Transform(pixelBearing, rotMat);

            // Compute screen position.
            auto relPixelPos = adjPixelOffset + adjPixelBearing;
            auto relPos      = (relPixelPos * fontScaleFactor) * text.Scale;
            auto pos         = adjTextPos + relPos;

            // Compute scale.
            auto relScale = Vector2((float)(shapedGlyph.Attribs.AtlasSize.x) / (float)(shapedGlyph.Attribs.AtlasSize.y), 1.0f) *
                            Vector2((float)(shapedGlyph.Attribs.AtlasSize.y) / (float)font->GetPointSize());
            auto scale    = relScale * text.Scale;

            // Concatenate name for texture atlas containing glyph.
            auto atlasName = text.FontName + std::to_string(shapedGlyph.Attribs.AtlasIdx);

            auto AddGlyph = [&](const Vector2& offset, const Color& color, int depth, bool hasGradient)
            {
                if (_glyphs2d.size() >= GLYPH_2D_COUNT_MAX)
                {
                    Debug::Log("Attempted to submit 2D glyph to full container.", Debug::LogLevel::Warning, Debug::LogMode::Debug);
                    return false;
                }

                auto glyph = Glyph2d::CreateGlyph2d(shapedGlyph, hasGradient,
                                                    atlasName, uvMin, uvMax,
                                                    pos + offset, text.Rotation, scale, color,
                                                    depth, ScaleMode::ShortEdge);
                _glyphs2d.push_back(glyph);

                return true;
            };

            // Submit 2D glyph.
            if (!AddGlyph(Vector2::Zero, color, text.Depth, text.Style == TextStyle::Gradient))
            {
                return false;
            }

            // Submit 2D drop shadow glyph.
            if (text.HasShadow)
            {
                // @todo Scale offset.
                auto adjShadowOffset = Vector2::Transform(SHADOW_OFFSET, rotMat);
                if (!AddGlyph(adjShadowOffset, SHADOW_COLOR, text.Depth + 1, false))
                {
                    return false;
                }
            }

            // Update horizontal offset.
            pixelOffset.x += shapedGlyph.Spacing + text.Tracking;
        }

        return true;
    }

    void RendererBase::SubmitDebugGui(std::function<void()> drawFunc)
    {
        if (_doubleBuffer.Active.DebugGuiDrawCalls.size() >= DEBUG_GUI_COUNT_MAX)
        {
            Debug::Log("Attempted to submit debug GUI draw call to full container.", Debug::LogLevel::Warning, Debug::LogMode::Debug);
            return;
        }

        const auto& options = g_App.GetOptions();
        if (!options->EnablePowerMode)
        {
            return;
        }

        _doubleBuffer.Active.DebugGuiDrawCalls.push_back(drawFunc);
        return;
    }

    void RendererBase::SubmitDebugLine(const Vector2& from, const Vector2& to, const Color& color, ScaleMode scaleMode, Debug::Page page)
    {
        if (!Debug::CheckPage(page))
        {
            return;
        }

        auto line = Shape2d::CreateLine(from, to, color, color, 0, scaleMode, BlendMode::Add);
        _doubleBuffer.Active.DebugShapes2d.push_back(line);
    }

    void RendererBase::SubmitDebugLine(const Vector3& from, const Vector3& to, const Color& color, Debug::Page page)
    {
        if (!Debug::CheckPage(page))
        {
            return;
        }

        auto line = Primitive3d::CreateDebugLine(from, to, color);
        _doubleBuffer.Active.DebugPrimitives3d.push_back(line);
    }

    void RendererBase::SubmitDebugTriangle(const Vector2& vert0, const Vector2& vert1, const Vector2& vert2, const Color& color, ScaleMode scaleMode, Debug::Page page)
    {
        if (!Debug::CheckPage(page))
        {
            return;
        }

        auto tri = Shape2d::CreateTriangle(vert0, vert1, vert2, color, color, color, 0, scaleMode, BlendMode::Add);
        _doubleBuffer.Active.DebugShapes2d.push_back(tri);
    }

    void RendererBase::SubmitDebugTriangle(const Vector3& vert0, const Vector3& vert1, const Vector3& vert2, const Color& color, Debug::Page page)
    {
        if (!Debug::CheckPage(page))
        {
            return;
        }

        auto tri = Primitive3d::CreateDebugTriangle(vert0, vert1, vert2, color);
        _doubleBuffer.Active.DebugPrimitives3d.push_back(tri);
    }

    void RendererBase::InitializeDoubleBuffer()
    {
        auto ReserveMemory = [](DoubleBuffer::Data& data)
        {
            data.Primitives2d.reserve(SHAPE_2D_COUNT_MAX + 
                                      SPRITE_2D_COUNT_MAX + 
                                      GLYPH_2D_COUNT_MAX);
        };

        ReserveMemory(_doubleBuffer.Active);
        ReserveMemory(_doubleBuffer.Render);

        _shapes2d.reserve(SHAPE_2D_COUNT_MAX);
        _sprites2d.reserve(SPRITE_2D_COUNT_MAX);
        _glyphs2d.reserve(GLYPH_2D_COUNT_MAX);
    }

    void RendererBase::ProcessSprites2d()
    {
        for (const auto& sprite : _sprites2d)
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

            // Compute relative vertex positions.
            auto rotMat  = Matrix::CreateRotationZ(-sprite.Rotation);
            auto relPos0 = Vector2::Transform(Vector2(-sprite.Scale.x, sprite.Scale.y) + offset, rotMat);
            auto relPos1 = Vector2::Transform(sprite.Scale                             + offset, rotMat);
            auto relPos2 = Vector2::Transform(Vector2(sprite.Scale.x, -sprite.Scale.y) + offset, rotMat);
            auto relPos3 = Vector2::Transform(-sprite.Scale                            + offset, rotMat);

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
                auto lock = ParallelLock(_primitives2dMutex);

                _doubleBuffer.Active.Primitives2d.push_back(Primitive2d
                {
                    .Vertices =
                    {
                        { pos0, sprite.Col0, uv0 },
                        { pos1, sprite.Col1, uv1 },
                        { pos2, sprite.Col2, uv2 },
                        { pos3, sprite.Col3, uv3 }
                    },
                    .Depth       = sprite.Depth,
                    .TextureName = sprite.TextureName,
                    .RenderStg   = RenderStage::Sprite2d,
                    .BlendMd     = sprite.BlendMd,
                    .Uniform     = UniformSprite2d
                    {
                        .UseTexture  = true, 
                        .IsFastAlpha = sprite.BlendMd == BlendMode::FastAlpha
                    }
                });
            }
        }
    }

    void RendererBase::ProcessShapes2d()
    {
        for (const auto& shape : _shapes2d)
        {
            // Triangle.
            if (shape.Vertices.size() == TRI_VERTEX_COUNT)
            {
                // Compute vertex positions.
                auto pos0 = ConvertScreenPercentToNdc(Vector2(shape.Vertices[0].Position.x, shape.Vertices[0].Position.y));
                auto pos1 = ConvertScreenPercentToNdc(Vector2(shape.Vertices[1].Position.x, shape.Vertices[1].Position.y));
                auto pos2 = ConvertScreenPercentToNdc(Vector2(shape.Vertices[2].Position.x, shape.Vertices[2].Position.y));

                // Add 2D primitive.
                // @lock Restrict 2D primitives access.
                {
                    auto lock = ParallelLock(_primitives2dMutex);

                    _doubleBuffer.Active.Primitives2d.push_back(Primitive2d
                    {
                        .Vertices =
                        {
                            Vertex2d{ pos0, shape.Vertices[0].Col, Vector2::Zero },
                            Vertex2d{ pos1, shape.Vertices[1].Col, Vector2::Zero },
                            Vertex2d{ pos2, shape.Vertices[2].Col, Vector2::Zero },
                        },
                        .Depth       = shape.Depth,
                        .TextureName = {},
                        .RenderStg   = RenderStage::Sprite2d,
                        .BlendMd     = shape.BlendMd,
                        .Uniform     = UniformSprite2d
                        {
                            .UseTexture  = false, 
                            .IsFastAlpha = shape.BlendMd == BlendMode::FastAlpha
                        }
                    });
                }
            }
            // Line or quad.
            else if (shape.Vertices.size() == QUAD_VERTEX_COUNT)
            {
                // Compute vertex positions.
                auto pos0 = ConvertScreenPercentToNdc(Vector2(shape.Vertices[0].Position.x, shape.Vertices[0].Position.y));
                auto pos1 = ConvertScreenPercentToNdc(Vector2(shape.Vertices[1].Position.x, shape.Vertices[1].Position.y));
                auto pos2 = ConvertScreenPercentToNdc(Vector2(shape.Vertices[2].Position.x, shape.Vertices[2].Position.y));
                auto pos3 = ConvertScreenPercentToNdc(Vector2(shape.Vertices[3].Position.x, shape.Vertices[3].Position.y));

                // Add 2D primitive.
                // @lock Restrict 2D primitives access.
                {
                    auto lock = ParallelLock(_primitives2dMutex);

                    _doubleBuffer.Active.Primitives2d.push_back(Primitive2d
                    {
                        .Vertices =
                        {
                            Vertex2d{ pos0, shape.Vertices[0].Col, Vector2::Zero },
                            Vertex2d{ pos1, shape.Vertices[1].Col, Vector2::Zero },
                            Vertex2d{ pos2, shape.Vertices[2].Col, Vector2::Zero },
                            Vertex2d{ pos3, shape.Vertices[3].Col, Vector2::Zero }
                        },
                        .Depth       = shape.Depth,
                        .TextureName = {},
                        .RenderStg   = RenderStage::Sprite2d,
                        .BlendMd     = shape.BlendMd,
                        .Uniform     = UniformSprite2d
                        {
                            .UseTexture  = false, 
                            .IsFastAlpha = shape.BlendMd == BlendMode::FastAlpha
                        }
                    });
                }
            }
        }
    }

    void RendererBase::ProcessGlyphs2d()
    {
        for (const auto& glyph : _glyphs2d)
        {
            // @todo Apply scale mode later.
            //auto pos = GetAspectCorrectScreenPosition(Vector2(vert.Position.x, vert.Position.y), sprite.ScaleMd);
            auto ndc = ConvertScreenPercentToNdc(glyph.Position);

            // Set alignment offset.
            auto offset = Vector2(glyph.Scale.x, glyph.Scale.y);

            // Compute relative vertex positions.
            auto rotMat  = Matrix::CreateRotationZ(-glyph.Rotation);
            auto relPos0 = Vector2::Transform(Vector2(-glyph.Scale.x, glyph.Scale.y) + offset, rotMat);
            auto relPos1 = Vector2::Transform(glyph.Scale                            + offset, rotMat);
            auto relPos2 = Vector2::Transform(Vector2(glyph.Scale.x, -glyph.Scale.y) + offset, rotMat);
            auto relPos3 = Vector2::Transform(-glyph.Scale                           + offset, rotMat);

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
                auto lock = ParallelLock(_primitives2dMutex);

                _doubleBuffer.Active.Primitives2d.push_back(Primitive2d
                {
                    .Vertices =
                    {
                        Vertex2d{ pos0, glyph.Col, uv0 },
                        Vertex2d{ pos1, glyph.Col, uv1 },
                        Vertex2d{ pos2, glyph.Col, uv2 },
                        Vertex2d{ pos3, glyph.Col, uv3 }
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
    }

    void RendererBase::SortRenderBufferData()
    {
        auto& executor = g_App.GetExecutor();

        auto sortTasks = ParallelTasks
        {
            // Sort 2D primitives. @todo Use sort keys?
            [&]()
            {
                Sort(_doubleBuffer.Render.Primitives2d, [](const Primitive2d& prim0, const Primitive2d& prim1)
                {
                    return prim0.Depth > prim1.Depth;
                });
            }
        };
        executor.AddTasks(sortTasks).wait();
    }

    void RendererBase::DrawFrame()
    {
        // @todo Refactor into a render graph?
        Draw3dScene();
        DrawDither();
        Draw2dScene();
        DrawPostProcess();
        DrawViewport();
        DrawPowerMenu();
    }

    std::unique_ptr<RendererBase> CreateRenderer(RendererType type)
    {
        switch (type)
        {
            case RendererType::SdlGpu:
            {
                return std::make_unique<SdlGpu::Renderer>();
            }
        }

        return nullptr;
    }
}
