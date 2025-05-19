#pragma once

#include "Engine/Renderer/Base.h"

namespace Silent::Renderer
{
    class OpenGlRenderer : public RendererBase
    {
    private:
        // Fields

        SDL_GLContext _context        = nullptr;
        uint          _vertexShader   = 0;
        uint          _fragmentShader = 0;
        uint          _shaderProgram  = 0;
        uint          _vao            = 0;
        uint          _vbo            = 0;

    public:
        // Constructors

        OpenGlRenderer() = default;

        // Utilities

        void Initialize(SDL_Window& window) override;
        void Deinitialize() override;
        void Update() override;
        void SaveScreenshot() const override;

    private:
        // Utilities
    
        void UpdateViewport();
        void DrawFrame();
        void DrawDebugGui();
        void DrawDebugObjects();

        void CreateShaderProgram();
        void CreateVertexBuffer();
        void CreateDebugGui();
    };
}
