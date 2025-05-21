#include "Framework.h"
#include "Engine/Renderer/Backends/OpenGl/OpenGl.h"

#include "Engine/Application.h"
#include "Engine/Renderer/Backends/OpenGl/ElementArrayBuffer.h"
#include "Engine/Renderer/Backends/OpenGl/Shader.h"
#include "Engine/Renderer/Backends/OpenGl/VertexArray.h"
#include "Engine/Renderer/Backends/OpenGl/VertexBuffer.h"
#include "Engine/Renderer/Base.h"

namespace Silent::Renderer
{
    static float VERTICES[] =
    {
        -0.5f, -0.5f * (float)sqrt(3) / 3,     0.0f,
        0.5f, -0.5f * (float)sqrt(3) / 3,     0.0f,
        0.0f,  0.5f * (float)sqrt(3) * 2 / 3, 0.0f,
        -0.5f / 2, 0.5f * float(sqrt(3)) / 6, 0.0f,
        0.5f / 2, 0.5f * float(sqrt(3)) / 6, 0.0f,
        0.0f, -0.5f * float(sqrt(3)) / 3, 0.0f
    };

    static uint VERTEX_INDICES[] =
    {
        0, 3, 5, // Lower left triangle
        3, 2, 4, // Upper triangle
        5, 4, 1  // Lower right triangle
    };

    void OpenGlRenderer::Initialize(SDL_Window& window)
    {
        _window = &window;

        // Create OpenGL context.
        _context = SDL_GL_CreateContext(_window);
        if (!_context)
        {
            throw std::runtime_error("Failed to create OpenGL context: " + std::string(SDL_GetError()));
        }

        // Load OpenGL functions using GLAD.
        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
        {
            throw std::runtime_error("Failed to load OpenGL functions.");
        }

        // Enable VSync.
        SDL_GL_SetSwapInterval(1);

        auto res = Vector2i::Zero;
        SDL_GetWindowSizeInPixels(_window, &res.x, &res.y);

        // Basic setup.
        glViewport(0, 0, res.x, res.y);
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        _shader.Initialize("Shaders/Default.vert", "Shaders/Default.frag");

        _vertexArray.Initialize();
        _vertexArray.Bind();

        _vertexBuffer.Initialize(VERTICES, sizeof(VERTICES));
        _elementArray.Initialize(VERTEX_INDICES, sizeof(VERTEX_INDICES));

        _vertexArray.LinkVertexBuffer(_vertexBuffer, 0);
        _vertexArray.Unbind();
        _vertexBuffer.Unbind();
        _elementArray.Unbind();

        //CreateShaderProgram();
        CreateDebugGui();
    }

    void OpenGlRenderer::Deinitialize()
    {
        _vertexArray.Delete();
        _vertexBuffer.Delete();
        _elementArray.Delete();
        _shader.Delete();
    }

    void OpenGlRenderer::Update()
    {
        // Update viewport.
        UpdateViewport();

        // Render.
        DrawFrame();
        DrawDebugGui();

        // Swap buffers.
        SDL_GL_SwapWindow(_window);

        // Clear scene. TODO
    }

    void OpenGlRenderer::SaveScreenshot() const
    {
        auto res = Vector2i::Zero;
        SDL_GetWindowSizeInPixels(_window, &res.x, &res.y);
        
        // Fill buffer with pixel data from framebuffer (RGBA format with 4 bytes per pixel).
        auto pixels = std::vector<uint8>((res.x * res.y) * 4);
        glReadPixels(0, 0, res.x, res.y, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
        
        // Flip image vertically to account for OpenGL coordinate system.
        for (int y = 0; y < (res.y / 2); y++)
        {
            for (int x = 0; x < (res.x * 4); x++)
            {
                std::swap(pixels[(y * res.x) + x], pixels[(((res.y - y) - 1) * res.x) + x]);
            }
        }

        // TODO: Write file.
    }

    void OpenGlRenderer::UpdateViewport()
    {
        // Clear screen.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Resize viewport if window is resized.
        if (_isResized)
        {
            auto res = Vector2i::Zero;
            SDL_GetWindowSizeInPixels(_window, &res.x, &res.y);
            glViewport(0, 0, res.x, res.y);
            _isResized = false;
        }
    }

    void OpenGlRenderer::DrawFrame()
    {
        glClearColor(0.07f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        _shader.Activate();
        _vertexArray.Bind();
        glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
    }

    void OpenGlRenderer::DrawDebugGui()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        for (auto& drawFunc : _debugGuiDrawCalls)
        {
            drawFunc();
        }
        _debugGuiDrawCalls.clear();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void OpenGlRenderer::DrawDebugObjects()
    {
        // TODO
        _debugLines.clear();
        _debugTriangles.clear();
    }

    void OpenGlRenderer::CreateDebugGui()
    {
        constexpr char VERSION[] = "#version 460";

        ImGui::CreateContext();
        ImGui_ImplSDL3_InitForOpenGL(_window, _context);
        ImGui_ImplOpenGL3_Init(VERSION);
    }
}
