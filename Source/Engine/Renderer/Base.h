#pragma once

namespace Silent::Renderer
{
    enum class DebugPage
    {
        None
    };

    class RendererBase
    {
    public:
        // Constructors

        RendererBase() = default;

        // Utilities

        virtual void Initialize(SDL_Window& window) = 0;
        virtual void Deinitialize() = 0;
        virtual void Update() = 0;
        
        virtual void SignalResizedFramebuffer() = 0;
        virtual void SaveScreenshot() const = 0;
        virtual void SubmitGui(std::function<void()> drawFunc) = 0;

        // Debug

        /*virtual void CreateLine(const Vector3& from, const Vector3& to, const Color& color, DebugPage page = DebugPage::None) = 0;
        virtual void CreateTriangle(const Vector3& vert0, const Vector3& vert1, const Vector3& vert2, const Color& color, DebugPage page = DebugPage::None) = 0;
        virtual void CreateTarget(const Vector3& center, const Quaternion& rot, float radius, const Color& color, DebugPage page = DebugPage::None) = 0;
        virtual void CreateBox(const AxisAlignedBoundingBox& aabb, const Color& color, bool isWireframe = true, DebugPage page = DebugPage::None) = 0;
        virtual void CreateBox(const OrientedBoundingBox& obb, const Color& color, bool isWireframe = true, DebugPage page = DebugPage::None) = 0;
        virtual void CreateSphere(const BoundingSphere& sphere, const Color& color, bool isWireframe = true, DebugPage page = DebugPage::None) = 0;
        virtual void CreateCylinder(const Vector3& center, const Quaternion& rot, float radius, float length, const Color& color, bool isWireframe = true, DebugPage page = DebugPage::None) = 0;
        virtual void CreateCone(const Vector3& center, const Quaternion& rot, float radius, float length, const Color& color, bool isWireframe = true, DebugPage page = DebugPage::None) = 0;
    */};
}
