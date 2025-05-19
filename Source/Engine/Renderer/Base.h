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
        virtual void SubmitGui(std::function<void()> drawFunc) = 0;
        virtual void SaveScreenshot() const = 0;
    };
}
