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
        const auto& options = g_App.GetConfig().GetOptions();
        if (!options.EnableDebugMode)
        {
            return;
        }

        _debugGuiDrawCalls.push_back(drawFunc);
    }

    void RendererBase::SubmitDebugLine(const Vector3& from, const Vector3& to, const Color& color, DebugPage page)
    {
        const auto& options = g_App.GetConfig().GetOptions();
        if (!options.EnableDebugMode || (page != g_App.GetDebugPage() && page != DebugPage::None))
        {
            return;
        }

        auto line = Line
        {
            .From  = from,
            .To    = to,
            .Color = color.ToVector4()
        };
        _debugLines.push_back(line);
    }

    void RendererBase::SubmitDebugTriangle(const Vector3& vert0, const Vector3& vert1, const Vector3& vert2, const Color& color, DebugPage page)
    {
        const auto& options = g_App.GetConfig().GetOptions();
        if (!options.EnableDebugMode || (page != g_App.GetDebugPage() && page != DebugPage::None))
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
            .Color = color.ToVector4()
        };
        _debugTriangles.push_back(tri);
    }

    void RendererBase::SubmitDebugTarget(const Vector3& center, const Quaternion& rot, float radius, const Color& color, DebugPage page)
    {
        const auto& options = g_App.GetConfig().GetOptions();
        if (!options.EnableDebugMode || (page != g_App.GetDebugPage() && page != DebugPage::None))
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

    void RendererBase::SubmitDebugBox(const OrientedBoundingBox& box, const Color& color, bool isWireframe, DebugPage page)
    {
        const auto& options = g_App.GetConfig().GetOptions();
        if (!options.EnableDebugMode || (page != g_App.GetDebugPage() && page != DebugPage::None))
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

    void RendererBase::SubmitDebugSphere(const BoundingSphere& sphere, const Color& color, bool isWireframe, DebugPage page)
    {
        constexpr uint WIREFRAME_SEGMENT_COUNT = 24;

        const auto& options = g_App.GetConfig().GetOptions();
        if (!options.EnableDebugMode || (page != g_App.GetDebugPage() && page != DebugPage::None))
        {
            return;
        }

        // Wireframe.
        if (isWireframe)
        {
            // Draw circles in XY, YZ, and XZ planes.
            for (int plane = 0; plane < Vector3::AXIS_COUNT; plane++)
            {
                for (int i = 0; i < WIREFRAME_SEGMENT_COUNT; i++)
                {
                    float theta0 = (((float)i / WIREFRAME_SEGMENT_COUNT) * 2.0f) * PI;
                    float theta1 = (((float)(i + 1) / WIREFRAME_SEGMENT_COUNT) * 2.0f) * PI;

                    auto point0 = Vector3::Zero;
                    auto point1 = Vector3::Zero;
                    switch (plane)
                    {
                        // XY plane.
                        case 0:
                            point0 = sphere.Center + Vector3(sphere.Radius * glm::cos(theta0), sphere.Radius * glm::sin(theta0), 0.0f);
                            point1 = sphere.Center + Vector3(sphere.Radius * glm::cos(theta1), sphere.Radius * glm::sin(theta1), 0.0f);
                            break;

                        // YZ plane.
                        case 1:
                            point0 = sphere.Center + Vector3(0.0f, sphere.Radius * glm::cos(theta0), sphere.Radius * glm::sin(theta0));
                            point1 = sphere.Center + Vector3(0.0f, sphere.Radius * glm::cos(theta1), sphere.Radius * glm::sin(theta1));
                            break;

                        // ZX plane.
                        case 2:
                            point0 = sphere.Center + Vector3(sphere.Radius * glm::cos(theta0), 0.0f, sphere.Radius * glm::sin(theta0));
                            point1 = sphere.Center + Vector3(sphere.Radius * glm::cos(theta1), 0.0f, sphere.Radius * glm::sin(theta1));
                            break;
                    }
                    SubmitDebugLine(point0, point1, color, page);
                }
            }
        }
        // Solid.
        else
        {
            // TOO
        }
    }

    void RendererBase::SubmitDebugCylinder(const Vector3& center, const Quaternion& rot, float radius, float length, const Color& color, bool isWireframe, DebugPage page)
    {
        const auto& options = g_App.GetConfig().GetOptions();
        if (!options.EnableDebugMode || (page != g_App.GetDebugPage() && page != DebugPage::None))
        {
            return;
        }

        // Wireframe.
        if (isWireframe)
        {
            // TODO
        }
        // Solid.
        else
        {
            // TODO
        }
    }

    void RendererBase::SubmitDebugCone(const Vector3& center, const Quaternion& rot, float radius, float length, const Color& color, bool isWireframe, DebugPage page)
    {
        const auto& options = g_App.GetConfig().GetOptions();
        if (!options.EnableDebugMode || (page != g_App.GetDebugPage() && page != DebugPage::None))
        {
            return;
        }

        // Wireframe.
        if (isWireframe)
        {
            // TODO
        }
        // Solid.
        else
        {
            // TODO
        }
    }

    void RendererBase::SubmitDebugDiamond(const Vector3& center, const Quaternion& rot, float radius, float length, const Color& color, bool isWireframe, DebugPage page)
    {
        const auto& options = g_App.GetConfig().GetOptions();
        if (!options.EnableDebugMode || (page != g_App.GetDebugPage() && page != DebugPage::None))
        {
            return;
        }

        // Wireframe.
        if (isWireframe)
        {
            // TODO
        }
        // Solid.
        else
        {
            // TODO
        }
    }
}
