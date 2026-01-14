#include "Framework.h"
#include "Renderer/Renderer.h"

#include "Application.h"
#include "Renderer/Common/Objects/Primitive/Vertex2d.h"
#include "Renderer/Common/Objects/Primitive/Vertex3d.h"
#include "Renderer/Common/Objects/Primitive2d.h"
#include "Renderer/Common/Objects/Primitive3d.h"
#include "Renderer/Common/Objects/Scene/Glyph.h"
#include "Renderer/Common/Objects/Scene/Sprite2d.h"
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
        _doubleBuffer.Active.Primitives3d.clear();
        _doubleBuffer.Active.Sprites2d.clear();
        _doubleBuffer.Active.DebugPrimitives3d.clear();
        _doubleBuffer.Active.DebugGuiDrawCalls.clear();
    }

    void RendererBase::SignalResize()
    {
        _isResized = true;
    }

    bool RendererBase::Submit2dPrimitive(const Shape2d& prim)
    {
        if (_doubleBuffer.Active.Shapes2d.size() >= PRIMITIVE_2D_COUNT_MAX)
        {
            Debug::Log("Attampted to add 2D primitive to full container.", Debug::LogLevel::Warning, Debug::LogMode::Debug);
            return false;
        }

        _doubleBuffer.Active.Shapes2d.push_back(prim);
        return true;
    }

    bool RendererBase::SubmitScreenSprite(const std::string& assetName, const Vector2& uvMin, const Vector2& uvMax,
                                          const Vector2& pos, float rot, const Vector2& scale, const Color& color,
                                          int depth, AlignMode alignMode, ScaleMode scaleMode, BlendMode blendMode)
    {
        auto& assets = g_App.GetAssets();

        if (_doubleBuffer.Active.Sprites2d.size() >= SPRITE_2D_COUNT_MAX)
        {
            Debug::Log("Attampted to add 2D sprite to full container.", Debug::LogLevel::Warning, Debug::LogMode::Debug);
            return false;
        }

        const auto asset = assets.GetAsset(assetName);
        if (asset->Type != AssetType::Tim)
        {
            Debug::Log("Attempted to submit non-image asset as screen sprite.", Debug::LogLevel::Warning, Debug::LogMode::Debug);
            return false;
        }

        auto sprite = Sprite2d
        {
            .TextureName = assetName,
            .UvMin       = uvMin,
            .UvMax       = uvMax,
            .Position    = pos,
            .Rotation    = rot,
            .Scale       = scale,
            .Col         = color,
            .Depth       = depth,
            .AlignMd     = alignMode,
            .ScaleMd     = scaleMode,
            .BlendMd     = blendMode
        };
        _doubleBuffer.Active.Sprites2d.push_back(sprite);
        return true;
    }

    void RendererBase::SubmitDebugGui(std::function<void()> drawFunc)
    {
        if (_doubleBuffer.Active.DebugGuiDrawCalls.size() >= DEBUG_GUI_DRAW_CALL_COUNT_MAX)
        {
            Debug::Log("Attampted to add debug GUI draw call to full container.", Debug::LogLevel::Warning, Debug::LogMode::Debug);
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
        _doubleBuffer.Active.Shapes2d.reserve(PRIMITIVE_2D_COUNT_MAX);
        _doubleBuffer.Active.Sprites2d.reserve(SPRITE_2D_COUNT_MAX);

        _doubleBuffer.Render = _doubleBuffer.Active; 
    }

    void RendererBase::PrepareRenderBufferData()
    {
        auto& executor = g_App.GetExecutor();

        auto sortTasks = ParallelTasks
        {
            // @todo Is sorting like this on the CPU necessary?
            //[&]()
            //{
            //    // Sort 2D primitives by depth.
            //    Sort(_doubleBuffer.Render.Primitives2d, [](const Primitive2d& prim0, const Primitive2d& prim1)
            //    {
            //        return prim0.Depth > prim1.Depth; // @todo Weird reverse order necessary here.
            //    });
            //},
            [&]()
            {
                // Sort 2D sprites by depth.
                // @todo Sort based on other heuristics too. Use sort keys for speed?
                Sort(_doubleBuffer.Render.Sprites2d,
                [](const Sprite2d& sprite0, const Sprite2d& sprite1)
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
