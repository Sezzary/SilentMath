#pragma once

#include "Engine/Renderer/Base.h"

namespace Silent::Renderer
{
    class OpenGlRenderer : public RendererBase
    {
    private:
        // Fields

        SDL_GLContext _glContext = nullptr;

        uint _vertexShader   = 0;
        uint _fragmentShader = 0;
        uint _shaderProgram  = 0;
        uint _vao            = 0;
        uint _vbo            = 0;

    public:
        // Constructors

        OpenGlRenderer() = default;

        // Utilities

        void Initialize(SDL_Window& window) override;
        void Deinitialize() override;
        void Update() override;
        
        void SignalResize() override;
        void SaveScreenshot() const override;
        void SubmitDebugGui(std::function<void()> drawFunc) override;

    private:
        // Utilities
    
        void InitializeDebugGui();
        void UpdateViewport();
        void DrawFrame();
        void DrawDebugGui();
    };
}
