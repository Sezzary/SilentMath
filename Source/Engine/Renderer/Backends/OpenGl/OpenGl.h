#pragma once

#include "Engine/Renderer/Backends/OpenGl/ElementBuffer.h"
#include "Engine/Renderer/Backends/OpenGl/Shaders.h"
#include "Engine/Renderer/Backends/OpenGl/Texture.h"
#include "Engine/Renderer/Backends/OpenGl/VertexArray.h"
#include "Engine/Renderer/Backends/OpenGl/VertexBuffer.h"
#include "Engine/Renderer/Backends/OpenGl/View.h"
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

        SDL_GLContext        _context       = nullptr;
        ShaderProgramManager _shaders       = ShaderProgramManager();
        VertexArrayObject    _vertexArray   = VertexArrayObject();
        VertexBufferObject   _vertexBuffer  = VertexBufferObject();
        ElementBufferObject  _elementBuffer = ElementBufferObject();
        View                 _view          = View();

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
        void DrawGui();
        void DrawDebugGui();

        void DrawDebugObjects();

        void CreateShaderProgram();
        void CreateDebugGui();
    };
}
