#pragma once

#include "Engine/Renderer/Base.h"

namespace Silent::Renderer
{
    class OpenGlRenderer : public RendererBase
    {
    private:
        // Fields
        
        SDL_Window*                        _window               = nullptr;
        bool                               _isFramebufferResized = false;
        std::vector<std::function<void()>> _guiDrawCalls         = {};

    public:
        // Constructors

        OpenGlRenderer() = default;

        // Utilities

        void Initialize(SDL_Window& window) override;
        void Deinitialize() override;
        void Update() override;
        
        void SignalResizedFramebuffer() override;
        void SaveScreenshot() const override;
        void SubmitGui(std::function<void()> drawFunc) override;

    private:
        // Utilities
    
        void InitializeDebugGui();
        void DrawDebugGui();
        void DrawFrame();
    };
}
