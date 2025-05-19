#pragma once

namespace Silent::Renderer
{
    enum class DebugPage
    {
        None
    };

    class RendererBase
    {
    protected:
        // Fields

        SDL_Window*                        _window            = nullptr;
        bool                               _isResized         = false;
        std::vector<std::function<void()>> _debugGuiDrawCalls = {};

    public:
        // Constructors

        RendererBase() = default;

        // Utilities

        virtual void Initialize(SDL_Window& window) = 0;
        virtual void Deinitialize() = 0;
        virtual void Update() = 0;
        
        virtual void SignalResize() = 0;
        virtual void SaveScreenshot() const = 0;
        virtual void SubmitDebugGui(std::function<void()> drawFunc) = 0;

        // Debug

        /*virtual void SubmitLine(const Vector3& from, const Vector3& to, const Color& color, DebugPage page = DebugPage::None) = 0;
        virtual void SubmitTriangle(const Vector3& vert0, const Vector3& vert1, const Vector3& vert2, const Color& color, DebugPage page = DebugPage::None) = 0;
        virtual void SubmitTarget(const Vector3& center, const Quaternion& rot, float radius, const Color& color, DebugPage page = DebugPage::None) = 0;
        virtual void SubmitBox(const AxisAlignedBoundingBox& aabb, const Color& color, bool isWireframe = true, DebugPage page = DebugPage::None) = 0;
        virtual void SubmitBox(const OrientedBoundingBox& obb, const Color& color, bool isWireframe = true, DebugPage page = DebugPage::None) = 0;
        virtual void SubmitSphere(const BoundingSphere& sphere, const Color& color, bool isWireframe = true, DebugPage page = DebugPage::None) = 0;
        virtual void SubmitCylinder(const Vector3& center, const Quaternion& rot, float radius, float length, const Color& color, bool isWireframe = true, DebugPage page = DebugPage::None) = 0;
        virtual void SubmitCone(const Vector3& center, const Quaternion& rot, float radius, float length, const Color& color, bool isWireframe = true, DebugPage page = DebugPage::None) = 0;
    */

    };
}
