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
#include "Renderer/Common/Resources/Scene/Triangle3d.h"
#include "Renderer/Common/Utils.h"
#include "Renderer/Common/View.h"
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

    Vector2i RendererBase::GetViewportResolution() const
    {
        const auto& options = g_App.GetOptions();

        auto res = g_App.GetWindowResolution();
        return res;

        // @todo Render scale should be a post-process instead?
        switch (options->RenderScale)
        {
            case RenderScaleType::Original:
            {
                res = RETRO_SCREEN_SPACE_RES.ToVector2i();
                break;
            }
            case RenderScaleType::DoubleOriginal:
            {
                res = RETRO_SCREEN_SPACE_RES.ToVector2i() * 2.0f;
                break;
            }
            case RenderScaleType::Native:
            {
                break;
            }
        }

        return res;
    }

    float RendererBase::GetViewportAspectRatio() const
    {
        auto res = GetViewportResolution().ToVector2();
        return res.x / res.y;
    }

    const BoundingFrustum& RendererBase::GetViewFrustum() const
    {
        return _view.Frustum;
    }

    void RendererBase::UpdateView()
    {
        // @todo
    }

    void RendererBase::PrepareFrameData()
    {
        auto& executor = g_App.GetExecutor();

        // @todo Using parallelism here causes flickering. Why if lock guards are in place??
        // Generate active buffer data.
        //auto tasks = ParallelTasks
        //{
        //    TASK(ProcessShapes2d()),
        //    TASK(ProcessSprites2d()),
        //    TASK(ProcessGlyphs2d()),
        //    TASK(ProcessTriangles3d())
        //};
        //executor.AddTasks(tasks).wait();
        ProcessShapes2d();
        ProcessSprites2d();
        ProcessGlyphs2d();
        ProcessTriangles3d();

        // Swap double buffer.
        std::swap(_doubleBuffer.Render, _doubleBuffer.Active);
        _doubleBuffer.Active.IsResized     = false;
        _doubleBuffer.Active.DrawCallCount = 0;
        _doubleBuffer.Active.ImmediatePrimitives2d.clear();
        _doubleBuffer.Active.ImmediatePrimitives3d.clear();
        _doubleBuffer.Active.DebugGuiDrawCalls.clear();
        _doubleBuffer.Active.TextureUploadQueue.clear();
        _doubleBuffer.Active.TextureReleaseQueue.clear();
        _doubleBuffer.Active.MeshUploadQueue.clear();
        _doubleBuffer.Active.MeshReleaseQueue.clear();

        // @debug Swap debug messages.
        Debug::g_Work.PrevMessages = Debug::g_Work.Messages;
        Debug::g_Work.Messages.clear();
    }

    void RendererBase::SignalResize()
    {
        _doubleBuffer.Active.IsResized = true;
    }

    void RendererBase::QueueTextureUpload(const std::string& assetName)
    {
        _doubleBuffer.Active.TextureUploadQueue.push_back(assetName);
    }

    void RendererBase::QueueTextureRelease(const std::string& assetName)
    {
        _doubleBuffer.Active.TextureReleaseQueue.push_back(assetName);
    }

    void RendererBase::QueueMeshUpload(const std::string& assetName)
    {
        _doubleBuffer.Active.MeshUploadQueue.push_back(assetName);
    }

    void RendererBase::QueueMeshRelease(const std::string& assetName)
    {
        _doubleBuffer.Active.MeshReleaseQueue.push_back(assetName);
    }

    bool RendererBase::SubmitShape2d(const Shape2d& shape)
    {
        if (_sceneObjects.Shapes2d.size() >= SHAPE_2D_COUNT_MAX)
        {
            Debug::Log("Attempted to submit 2D shape to full container.",
                       Debug::LogLevel::Warning, Debug::LogMode::Debug);
            return false;
        }

        _sceneObjects.Shapes2d.push_back(shape);
        return true;
    }

    bool RendererBase::SubmitSprite2d(const Sprite2d& sprite)
    {
        auto& assets = g_App.GetAssets();

        if (_sceneObjects.Sprites2d.size() >= SPRITE_2D_COUNT_MAX)
        {
            Debug::Log("Attempted to submit 2D sprite to full container.",
                       Debug::LogLevel::Warning, Debug::LogMode::Debug);
            return false;
        }

        // @todo Handle this somehow somewhere, but not here. Sprite textures don't
        // have to come from streamable assets, e.g. font atlases.
        //const auto* asset = assets.GetAsset(sprite.TextureName);
        //if (asset->Type != AssetType::Tim)
        //{
        //    Debug::Log("Attempted to submit non-image asset as screen sprite.",
        //               Debug::LogLevel::Warning, Debug::LogMode::Debug);
        //    return false;
        //}

        _sceneObjects.Sprites2d.push_back(sprite);
        return true;
    }

    bool RendererBase::SubmitText2d(const Text2d& text)
    {
        constexpr auto    SHADOW_COLOR  = Color::From8Bit(16, 16, 16);
        static const auto SHADOW_OFFSET = SCREEN_SPACE_RES / Vector2(RETRO_SCREEN_SPACE_RES.y);

        // Compute transformation parameters.
        auto  rotMat           = Matrix::CreateRotationZ(text.Rotation);
        auto  fontScaleFactor  = SCREEN_SPACE_RES / (float)text.Font->GetPointSize();
        float heightScale      = (text.StyleFlags & (int)TextStyleFlags::HalfHeight) ? 0.5f : 1.0f;
        auto  textScale        = Vector2(1.0f, heightScale) * text.Scale;
        auto  textSize         = (Vector2(text.Shape.Width, (float)text.Font->GetPointSize()) * fontScaleFactor) * textScale;
        auto  aspectCorrection = GetScreenAspectCorrection(text.ScaleMd);

        // Compute text position.
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

        // Compute shadow offset.
        auto shadowOffset    = (SHADOW_OFFSET * Vector2(1.0f, heightScale)) * aspectCorrection;
        auto adjShadowOffset = Vector2::Transform(shadowOffset, rotMat);
        // @todo This version scales according to the internal pixel size of the font.
        //auto shadowOffset    = ((SHADOW_RETRO_PIXEL_OFFSET * fontScaleFactor) * textScale) * aspectCorrection;
        //auto adjShadowOffset = Vector2::Transform(shadowOffset, rotMat);

        // Run through shaped glyphs.
        auto pixelOffset = Vector2::Zero;
        for (const auto& shapedGlyph : text.Shape.Glyphs)
        {
            // Compute texture atlas UVs.
            auto uvMin = shapedGlyph.Attribs.AtlasPosition.ToVector2() / Vector2(Font::ATLAS_SIZE); 
            auto uvMax = uvMin + (shapedGlyph.Attribs.AtlasSize.ToVector2() / Vector2(Font::ATLAS_SIZE));

            // Compute rotated offset.
            auto adjPixelOffset = Vector2::Transform(pixelOffset, rotMat);

            // Compute rotated bearing.
            auto pixelBearing    = Vector2(shapedGlyph.Attribs.Bearing.x,
                                           shapedGlyph.Attribs.AtlasSize.y - shapedGlyph.Attribs.Bearing.y);
            auto adjPixelBearing = Vector2::Transform(pixelBearing, rotMat);

            // Compute screen position.
            auto relPixelPos = adjPixelOffset + adjPixelBearing;
            auto relPos      = (relPixelPos * fontScaleFactor) * textScale;
            auto pos         = adjTextPos + (relPos * aspectCorrection);

            // Compute scale.
            auto relScale = Vector2((float)shapedGlyph.Attribs.AtlasSize.x / (float)shapedGlyph.Attribs.AtlasSize.y, 1.0f) *
                            Vector2((float)shapedGlyph.Attribs.AtlasSize.y / (float)text.Font->GetPointSize());
            auto scale    = relScale * textScale;

            // Concatenate name for texture atlas containing glyph.
            auto atlasName = text.Font->GetName() + std::to_string(shapedGlyph.Attribs.AtlasIdx);

            auto AddGlyph = [&](const Vector2& offset, const Color& color, int depth, bool hasGradient)
            {
                if (_sceneObjects.Glyphs2d.size() >= GLYPH_2D_COUNT_MAX)
                {
                    Debug::Log("Attempted to submit 2D glyph to full container.",
                               Debug::LogLevel::Warning, Debug::LogMode::Debug);
                    return false;
                }

                auto glyph = Glyph2d::CreateGlyph2d(shapedGlyph, hasGradient,
                                                    atlasName, uvMin, uvMax,
                                                    pos + offset, text.Rotation, scale, color,
                                                    depth, text.ScaleMd);
                _sceneObjects.Glyphs2d.push_back(glyph);

                return true;
            };

            // Submit 2D glyph.
            if (!AddGlyph(Vector2::Zero, text.Col, text.Depth, text.StyleFlags & (int)TextStyleFlags::Gradient))
            {
                return false;
            }

            // Submit 2D drop shadow glyph.
            if (text.HasShadow)
            {
                auto shadowColor = SHADOW_COLOR;
                shadowColor.A()  = text.Col.A();
                if (!AddGlyph(adjShadowOffset, shadowColor, text.Depth + 1, false))
                {
                    return false;
                }
            }

            // Update horizontal offset.
            pixelOffset.x += shapedGlyph.Spacing + text.Tracking;
        }

        return true;
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
