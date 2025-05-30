#pragma once

#include "Engine/Renderer/Backends/OpenGl/ElementArrayBuffer.h"
#include "Engine/Renderer/Backends/OpenGl/Shader.h"
#include "Engine/Renderer/Backends/OpenGl/VertexArray.h"
#include "Engine/Renderer/Backends/OpenGl/VertexBuffer.h"
#include "Engine/Renderer/Base.h"

namespace Silent::Renderer
{
    constexpr char OPEN_GL_VERSION[] = "#version 460";

    class OpenGlRenderer : public RendererBase
    {
    private:
        // Constants

        static constexpr Color DEFAULT_COLOR = Color(0.2f, 0.2f, 0.2f, 1.0f);

        // Fields

        SDL_GLContext             _context      = nullptr;
        ShaderManager             _shader       = ShaderManager();
        VertexArrayManager        _vertexArray  = VertexArrayManager();
        VertexBufferManager       _vertexBuffer = VertexBufferManager();
        ElementArrayBufferManager _elementArray = ElementArrayBufferManager();
        GLuint                    _uniformId    = 0;

        Vector2i _imageRes               = Vector2i::Zero;
        int      _imageColorChannelCount = 0;
        uchar*   _imageBytes             = nullptr;
        GLuint   _textureId              = 0;
        GLuint   _textureUniformId       = 0;

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
        void CreateDebugGui();
    };
}
