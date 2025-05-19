#pragma once

#include "Engine/Renderer/Base.h"

namespace Silent::Renderer
{
    class OpenGlRenderer : public RendererBase
    {
    public:
        // Constructors

        OpenGlRenderer() = default;

        // Utilities

        void Initialize(SDL_Window& window) override;
        void Deinitialize() override;
        void Update() override;
        
        void SignalResizedFramebuffer() override;
        void SubmitGui(std::function<void()> drawFunc) override;
        void SaveScreenshot() const override;
    };
}
