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
        std::swap(_doubleBuffer.Render, _doubleBuffer.Active);

        _doubleBuffer.Active.DrawCallCount = 0;
        _doubleBuffer.Active.Shapes2d.clear();
        _doubleBuffer.Active.Sprites2d.clear();
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
        // @todo Improve `constexpr` compatibility of math classes.
        static const auto SHADOW_OFFSET = SCREEN_SPACE_RES * (1.0f / RETRO_SCREEN_SPACE_RES.y);
        constexpr auto SHADOW_COLOR = Color(0.0f, 0.0f, 1.0f); // @todo Actual colour is off-black.

        auto& fonts = g_App.GetFonts();

        // Get font.
        auto* font = fonts.GetFont(text.FontName);
        if (font == nullptr)
        {
            Debug::Log(Fmt("Attempted to submit 2D text with missing font `{}`.", text.FontName), Debug::LogLevel::Warning, Debug::LogMode::Debug);
            return false;
        }

        auto scaleFactor = SCREEN_SPACE_RES / font->GetPointSize();
        auto glyphOffset =  Vector2::Zero;

        // Run through shaped glyphs in text.
        auto shapedText = font->GetShapedText(text.Message);
        for (const auto& glyph : shapedText.Glyphs)
        {
            // Compute UVs.
            auto uvMin = glyph.Metadata.AtlasPosition.ToVector2() / Vector2(Font::ATLAS_SIZE); 
            auto uvMax = uvMin + (glyph.Metadata.AtlasSize.ToVector2() / Vector2(Font::ATLAS_SIZE));

            // Compute rotated offset.
            // @todo Not used yet. Check if this is correct.
            auto rotMat    = Matrix::CreateRotationZ(text.Rotation);
            auto rotOffset = Vector2::Transform(glyphOffset, rotMat);

            // @todo Align mode.

            // Compute glyph position.
            auto relPixelPos = glyphOffset + Vector2(glyph.Metadata.Bearing.x, glyph.Metadata.AtlasSize.y - glyph.Metadata.Bearing.y);
            auto relGlyphPos = (relPixelPos * scaleFactor) * text.Scale;

            // Compute glyph scale.
            auto glyphScale = Vector2((float)glyph.Metadata.AtlasSize.x / (float)glyph.Metadata.AtlasSize.y, 1.0f) *
                              Vector2((float)glyph.Metadata.AtlasSize.y / (float)font->GetPointSize());

            // Concatenate name for texture atlas containing glyph.
            auto atlasName = text.FontName + std::to_string(glyph.Metadata.AtlasIdx);

            // Submit 2D glyph sprite.
            auto glyphPos    = text.Position + relGlyphPos;
            auto glyphSprite = Sprite2d::CreateSprite2d(atlasName, uvMin, uvMax,
                                                        glyphPos, text.Rotation, glyphScale * text.Scale, Color(1.0f, 1.0f, 1.0f, text.Opacity),
                                                        text.Depth, AlignMode::BottomLeft, ScaleMode::LongEdge, BlendMode::FastAlpha);
            if (!SubmitSprite2d(glyphSprite))
            {
                return false;
            }

            // Submit 2D drop shadow sprite. @todo Needs special rotated offset.
            if (text.HasShadow)
            {
                auto shadowPos    = glyphPos + SHADOW_OFFSET;
                auto shadowSprite = Sprite2d::CreateSprite2d(atlasName, uvMin, uvMax,
                                                             shadowPos, text.Rotation, glyphScale * text.Scale, SHADOW_COLOR,
                                                             text.Depth + 1, AlignMode::BottomLeft, ScaleMode::LongEdge, BlendMode::FastAlpha);
                if (!SubmitSprite2d(shadowSprite))
                {
                    return false;
                }
            }

            // Update horizontal offset.
            glyphOffset.x += glyph.Kerning;
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

        _doubleBuffer.Render = _doubleBuffer.Active; 
    }

    void RendererBase::SortRenderBufferData()
    {
        auto& executor = g_App.GetExecutor();

        auto sortTasks = ParallelTasks
        {
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
