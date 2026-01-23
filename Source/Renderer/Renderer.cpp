#include "Framework.h"
#include "Renderer/Renderer.h"

#include "Application.h"
#include "Renderer/Common/Objects/Primitive/Vertex2d.h"
#include "Renderer/Common/Objects/Primitive/Vertex3d.h"
#include "Renderer/Common/Objects/Primitive3d.h"
#include "Renderer/Common/Objects/Scene/Shape2d.h"
#include "Renderer/Common/Objects/Scene/Sprite2d.h"
#include "Renderer/Common/Objects/Scene/Text2d.h"
#include "Renderer/Backends/SdlGpu/SdlGpu.h"
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

    void RendererBase::SwapDoubleBuffer()
    {
        // @todo Need to call `UpdateFontAtlasTextures` here. Backends need their own
        // pre-render data prep method.

        std::swap(_doubleBuffer.Render, _doubleBuffer.Active);

        _doubleBuffer.Active.DrawCallCount = 0;
        _doubleBuffer.Active.Shapes2d.clear();
        _doubleBuffer.Active.Sprites2d.clear();
        _doubleBuffer.Active.Glyphs2d.clear();
        _doubleBuffer.Active.DebugGuiDrawCalls.clear();

        _doubleBuffer.Active.Primitives3d.clear();
        _doubleBuffer.Active.DebugPrimitives3d.clear();
    }

    void RendererBase::SignalResize()
    {
        _isResized = true;
    }

    bool RendererBase::SubmitShape2d(const Shape2d& shape)
    {
        if (_doubleBuffer.Active.Shapes2d.size() >= SHAPE_2D_COUNT_MAX)
        {
            Debug::Log("Attempted to submit 2D shape to full container.", Debug::LogLevel::Warning, Debug::LogMode::Debug);
            return false;
        }

        _doubleBuffer.Active.Shapes2d.push_back(shape);
        return true;
    }

    bool RendererBase::SubmitSprite2d(const Sprite2d& sprite)
    {
        auto& assets = g_App.GetAssets();

        if (_doubleBuffer.Active.Sprites2d.size() >= SPRITE_2D_COUNT_MAX)
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

        _doubleBuffer.Active.Sprites2d.push_back(sprite);
        return true;
    }

    bool RendererBase::SubmitText2d(const Text2d& text)
    {
        constexpr auto SHADOW_COLOR    = Color::From8Bit(16,  16,  16);
        // @todo Improve `constexpr` compatibility of math classes.
        static const auto SHADOW_OFFSET = SCREEN_SPACE_RES * (1.0f / RETRO_SCREEN_SPACE_RES.y);

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
        auto textOffset = Vector2::One;
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
            auto uvMin  = shapedGlyph.Attribs.AtlasPosition.ToVector2() / Vector2(Font::ATLAS_SIZE); 
            auto uvMax  = uvMin + (shapedGlyph.Attribs.AtlasSize.ToVector2() / Vector2(Font::ATLAS_SIZE));
            auto uvSize = uvMax - uvMin;

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
            auto relScale = Vector2((float)shapedGlyph.Attribs.AtlasSize.x / (float)shapedGlyph.Attribs.AtlasSize.y, 1.0f) *
                            Vector2((float)shapedGlyph.Attribs.AtlasSize.y / (float)font->GetPointSize());
            auto scale    = relScale * text.Scale;

            // Concatenate name for texture atlas containing glyph.
            auto atlasName = text.FontName + std::to_string(shapedGlyph.Attribs.AtlasIdx);

            auto AddGlyph = [&](const Vector2& offset, const Color& color, int depth, bool hasGradient)
            {
                if (_doubleBuffer.Active.Glyphs2d.size() >= GLYPH_2D_COUNT_MAX)
                {
                    Debug::Log("Attempted to submit 2D glyph to full container.", Debug::LogLevel::Warning, Debug::LogMode::Debug);
                    return false;
                }

                auto glyph = Glyph2d::CreateGlyph2d(shapedGlyph, hasGradient,
                                                    atlasName, uvMin, uvMax,
                                                    pos + offset, text.Rotation, scale, color,
                                                    depth, ScaleMode::ShortEdge);
                _doubleBuffer.Active.Glyphs2d.push_back(glyph);

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
                auto adjShadowOffset = Vector2::Transform(SHADOW_OFFSET, rotMat);
                if (!AddGlyph(adjShadowOffset, SHADOW_COLOR, text.Depth + 1, false))
                {
                    return false;
                }
            }

            // Update horizontal offset.
            pixelOffset.x += shapedGlyph.Kerning * text.KerningScale;
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
        _doubleBuffer.Active.Shapes2d.reserve(SHAPE_2D_COUNT_MAX);
        _doubleBuffer.Active.Sprites2d.reserve(SPRITE_2D_COUNT_MAX);
        _doubleBuffer.Active.Glyphs2d.reserve(GLYPH_2D_COUNT_MAX);

        _doubleBuffer.Render = _doubleBuffer.Active; 
    }

    void RendererBase::SortRenderBufferData()
    {
        auto& executor = g_App.GetExecutor();

        auto sortTasks = ParallelTasks
        {
            // Sort 2D glyphs.
            [&]()
            {
                Sort(_doubleBuffer.Render.Glyphs2d, [](const Glyph2d& glyph0, const Glyph2d& glyph1)
                {
                    return glyph0.Depth > glyph1.Depth;
                });
            },
            // Sort 2D shapes.
            [&]()
            {
                Sort(_doubleBuffer.Render.Shapes2d, [](const Shape2d& shape0, const Shape2d& shape1)
                {
                    return shape0.Depth > shape1.Depth;
                });
            },
            // Sort 2D sprites.
            [&]()
            {
                // @todo Sort based on other heuristics too. Use sort keys for speed?
                Sort(_doubleBuffer.Render.Sprites2d, [](const Sprite2d& sprite0, const Sprite2d& sprite1)
                {
                    return sprite0.Depth > sprite1.Depth;
                });
            }
        };
        executor.AddTasks(sortTasks).wait();
    }

    std::unique_ptr<RendererBase> CreateRenderer(RendererType type)
    {
        switch (type)
        {
            case RendererType::SdlGpu:
            {
                return std::make_unique<SdlGpuRenderer>();
            }
        }

        return nullptr;
    }
}
