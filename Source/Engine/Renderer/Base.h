#pragma once

#include "Engine/Renderer/Structures/Line.h"
#include "Engine/Renderer/Structures/Triangle.h"

namespace Silent::Renderer
{
    class RendererBase
    {
    protected:
        // Fields

        SDL_Window* _window    = nullptr;
        bool        _isResized = false;

        std::vector<std::function<void()>> _debugGuiDrawCalls = {};
        std::vector<Line>                  _debugLines        = {};
        std::vector<Triangle>              _debugTriangles    = {};

    public:
        // Constructors

        RendererBase() = default;

        // Utilities

        virtual void Initialize(SDL_Window& window) = 0;
        virtual void Deinitialize() = 0;
        virtual void Update() = 0;
        virtual void SaveScreenshot() const = 0;
        
        void SignalResize();

        // Debug

        void SubmitDebugGui(std::function<void()> drawFunc);
        void SubmitDebugLine(const Vector3& from, const Vector3& to, const Color& color, DebugPage page);
        /*virtual void SubmitDebugTriangle(const Vector3& vert0, const Vector3& vert1, const Vector3& vert2, const Color& color, DebugPage page) = 0;
        virtual void SubmitDebugTarget(const Vector3& center, const Quaternion& rot, float radius, const Color& color, DebugPage page) = 0;
        virtual void SubmitDebugBox(const AxisAlignedBoundingBox& aabb, const Color& color, bool isWireframe, DebugPage page) = 0;
        virtual void SubmitDebugBox(const OrientedBoundingBox& obb, const Color& color, bool isWireframe, DebugPage page) = 0;
        virtual void SubmitDebugSphere(const BoundingSphere& sphere, const Color& color, bool isWireframe, DebugPage page) = 0;
        virtual void SubmitDebugCylinder(const Vector3& center, const Quaternion& rot, float radius, float length, const Color& color, bool isWireframe, DebugPage page) = 0;
        virtual void SubmitDebugCone(const Vector3& center, const Quaternion& rot, float radius, float length, const Color& color, bool isWireframe, DebugPage page) = 0;*/
    };
}
