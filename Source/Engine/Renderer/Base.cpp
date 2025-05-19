#include "Framework.h"
#include "Engine/Renderer/Base.h"

#include "Engine/Application.h"

namespace Silent::Renderer
{
    void RendererBase::SignalResize()
    {
        _isResized = true;
    }

    void RendererBase::SubmitDebugGui(std::function<void()> drawFunc)
    {
        if constexpr (IS_DEBUG)
        {
            _debugGuiDrawCalls.push_back(drawFunc);
        }
    }

    void RendererBase::SubmitDebugLine(const Vector3& from, const Vector3& to, const Color& color, DebugPage page)
    {
        if constexpr (IS_DEBUG)
        {
            if (page != g_App.GetDebugPage() && page != DebugPage::None)
            {
                return;
            }
    
            auto line = Line
            {
                .From  = from,
                .To    = to,
                .Color = Vector4(color.ToGlmVec4())
            };
            _debugLines.push_back(line);
        }
    }

    void RendererBase::SubmitDebugTriangle(const Vector3& vert0, const Vector3& vert1, const Vector3& vert2, const Color& color, DebugPage page)
    {
        if constexpr (IS_DEBUG)
        {
            if (page != g_App.GetDebugPage() && page != DebugPage::None)
            {
                return;
            }
    
            auto tri = Triangle
            {
                .Vertices =
                {
                    vert0,
                    vert1,
                    vert2
                },
                .Color = Vector4(color.ToGlmVec4())
            };
            _debugTriangles.push_back(tri);
        }
    }

    void RendererBase::SubmitDebugTarget(const Vector3& center, const Quaternion& rot, float radius, const Color& color, DebugPage page)
    {
        if constexpr (IS_DEBUG)
        {
            if (page != g_App.GetDebugPage() && page != DebugPage::None)
            {
                return;
            }
    
            auto rotMatrix = rot.ToRotationMatrix();

            auto from0 = center + Vector3::Transform(Vector3(radius,  0.0f, 0.0f), rotMatrix);
            auto to0   = center + Vector3::Transform(Vector3(-radius, 0.0f, 0.0f), rotMatrix);
            SubmitDebugLine(from0, to0, color, page);

            auto from1 = center + Vector3::Transform(Vector3(0.0f,  radius, 0.0f), rotMatrix);
            auto to1   = center + Vector3::Transform(Vector3(0.0f, -radius, 0.0f), rotMatrix);
            SubmitDebugLine(from1, to1, color, page);

            auto from2 = center + Vector3::Transform(Vector3(0.0f, 0.0f,  radius), rotMatrix);
            auto to2   = center + Vector3::Transform(Vector3(0.0f, 0.0f, -radius), rotMatrix);
            SubmitDebugLine(from2, to2, color, page);
        }
    }

    void RendererBase::SubmitDebugBox(const OrientedBoundingBox& box, const Color& color, bool isWireframe, DebugPage page)
    {
        if constexpr (IS_DEBUG)
        {
            if (page != g_App.GetDebugPage() && page != DebugPage::None)
            {
                return;
            }

            auto corners = box.GetCorners();

            // Wireframe.
            if (isWireframe)
            {
                SubmitDebugLine(corners[0], corners[1], color, page);
                SubmitDebugLine(corners[1], corners[2], color, page);
                SubmitDebugLine(corners[2], corners[3], color, page);
                SubmitDebugLine(corners[3], corners[0], color, page);

                SubmitDebugLine(corners[4], corners[5], color, page);
                SubmitDebugLine(corners[5], corners[6], color, page);
                SubmitDebugLine(corners[6], corners[7], color, page);
                SubmitDebugLine(corners[7], corners[4], color, page);

                SubmitDebugLine(corners[0], corners[4], color, page);
                SubmitDebugLine(corners[1], corners[5], color, page);
                SubmitDebugLine(corners[2], corners[6], color, page);
                SubmitDebugLine(corners[3], corners[7], color, page);
            }
            // Solid.
            else
            {
                SubmitDebugTriangle(corners[0], corners[1], corners[2], color, page);
                SubmitDebugTriangle(corners[0], corners[2], corners[3], color, page);

                SubmitDebugTriangle(corners[4], corners[5], corners[6], color, page);
                SubmitDebugTriangle(corners[4], corners[6], corners[7], color, page);

                SubmitDebugTriangle(corners[0], corners[1], corners[4], color, page);
                SubmitDebugTriangle(corners[1], corners[4], corners[5], color, page);

                SubmitDebugTriangle(corners[1], corners[2], corners[5], color, page);
                SubmitDebugTriangle(corners[2], corners[5], corners[6], color, page);

                SubmitDebugTriangle(corners[2], corners[3], corners[6], color, page);
                SubmitDebugTriangle(corners[3], corners[6], corners[7], color, page);

                SubmitDebugTriangle(corners[0], corners[3], corners[4], color, page);
                SubmitDebugTriangle(corners[3], corners[4], corners[7], color, page);
            }
        }
    }

    void RendererBase::SubmitDebugSphere(const BoundingSphere& sphere, const Color& color, bool isWireframe, DebugPage page)
    {
        if constexpr (IS_DEBUG)
        {
            if (page != g_App.GetDebugPage() && page != DebugPage::None)
            {
                return;
            }

            // TODO
        }
    }

    void RendererBase::SubmitDebugCylinder(const Vector3& center, const Quaternion& rot, float radius, float length, const Color& color, bool isWireframe, DebugPage page)
    {
        if constexpr (IS_DEBUG)
        {
            if (page != g_App.GetDebugPage() && page != DebugPage::None)
            {
                return;
            }

            // TODO
        }
    }

    void RendererBase::SubmitDebugCone(const Vector3& center, const Quaternion& rot, float radius, float length, const Color& color, bool isWireframe, DebugPage page)
    {
        if constexpr (IS_DEBUG)
        {
            if (page != g_App.GetDebugPage() && page != DebugPage::None)
            {
                return;
            }

            // TODO
        }
    }

    void RendererBase::SubmitDebugDiamond(const Vector3& center, const Quaternion& rot, float radius, float length, const Color& color, bool isWireframe, DebugPage page)
    {
        if constexpr (IS_DEBUG)
        {
            if (page != g_App.GetDebugPage() && page != DebugPage::None)
            {
                return;
            }

            // TODO
        }
    }
}
