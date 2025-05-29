#include "Framework.h"
#include "Engine/Renderer/Backends/OpenGl/OpenGl.h"

#include "Engine/Application.h"
#include "Engine/Renderer/Backends/OpenGl/ElementArrayBuffer.h"
#include "Engine/Renderer/Backends/OpenGl/Shader.h"
#include "Engine/Renderer/Backends/OpenGl/VertexArray.h"
#include "Engine/Renderer/Backends/OpenGl/VertexBuffer.h"
#include "Engine/Renderer/Base.h"
#include "Engine/Services/Configuration.h"
#include "Engine/Services/Time.h"

using namespace Silent::Services;

namespace Silent::Renderer
{
    static GLfloat VERTICES[] =
    {
        -0.5f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f, // Lower left corner
        0.5f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f, // Lower right corner
        0.0f,  0.5f * float(sqrt(3)) * 2 / 3, 0.0f,     1.0f, 0.6f,  0.32f, // Upper corner
        -0.25f, 0.5f * float(sqrt(3)) * 1 / 6, 0.0f,     0.9f, 0.45f, 0.17f, // Inner left
        0.25f, 0.5f * float(sqrt(3)) * 1 / 6, 0.0f,     0.9f, 0.45f, 0.17f, // Inner right
        0.0f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f  // Inner down
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
        //glEnable(GL_DEPTH_TEST);
        //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT);

        CreateShaderProgram();
        CreateDebugGui();

        _uniformId = glGetUniformLocation(_shader.Id, "scale");
    }

    void OpenGlRenderer::Deinitialize()
    {
        // Delete objects.
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
        constexpr uint COLOR_CHANNEL_COUNT = 3; // RGB.

        const auto& config = g_App.GetConfig();

        // Get window size.
        auto res = Vector2i::Zero;
        SDL_GetWindowSizeInPixels(_window, &res.x, &res.y);
        
        // Ensure directory exists.
        auto timestamp = GetCurrentDateString() + "_" + GetCurrentTimeString();
        auto filename  = (SCREENSHOT_FILENAME_BASE + timestamp) + PNG_FILE_EXT;
        auto path      = config.GetScreenshotsFolder() / filename;
        std::filesystem::create_directories(path.parent_path());

        // Capture screenshot.
        auto pixels = std::vector<uint8>((res.x * res.y) * COLOR_CHANNEL_COUNT);
        glReadPixels(0, 0, res.x, res.y, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

        // Flip pixels vertically.
        int rowSize = res.x * COLOR_CHANNEL_COUNT;
        for (int y = 0; y < (res.y / 2); y++)
        {
            int oppositeY = (res.y - y) - 1;
            for (int x = 0; x < rowSize; x++)
            {
                std::swap(pixels[(y * rowSize) + x], pixels[(oppositeY * rowSize) + x]);
            }
        }

        // Write screenshot file.
        if (stbi_write_png(path.string().c_str(), res.x, res.y, COLOR_CHANNEL_COUNT, pixels.data(), res.x * COLOR_CHANNEL_COUNT))
        {
            // TODO: Shutter SFX, postprocess flash effect, timeout?
            return;
        }

        Log("Failed to save screenshot.", LogLevel::Warning, LogMode::DebugRelease, true);
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
        glClearColor(DEFAULT_COLOR.R(), DEFAULT_COLOR.G(), DEFAULT_COLOR.B(), DEFAULT_COLOR.A());
        glClear(GL_COLOR_BUFFER_BIT);

        _shader.Activate();

        glUniform1f(_uniformId, 0.5f);

        _vertexArray.Bind();
        glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
    }

    void OpenGlRenderer::DrawDebugGui()
    {
        // If debug GUI is disabled, return early.
        const auto& options = g_App.GetConfig().GetOptions();
        if (!options.EnableDebugGui)
        {
            _debugGuiDrawCalls.clear();
            return;
        }

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

        // Clear object elements.
        _debugLines.clear();
        _debugTriangles.clear();
    }

    void OpenGlRenderer::CreateShaderProgram()
    {
        // Generate shader object.
        _shader.Initialize("Shaders/Default.vert", "Shaders/Default.frag");

        // Generate and bind vertex array object.
        _vertexArray.Initialize();
        _vertexArray.Bind();

        // Generate vertex buffer and element array objects.
        _vertexBuffer.Initialize(VERTICES, sizeof(VERTICES));
        _elementArray.Initialize(VERTEX_INDICES, sizeof(VERTEX_INDICES));

        // Link attributes.
        _vertexArray.LinkAttrib(_vertexBuffer, 0, 3, GL_FLOAT, sizeof(float) * 6, (void*)0);
        _vertexArray.LinkAttrib(_vertexBuffer, 1, 3, GL_FLOAT, sizeof(float) * 6, (void*)(sizeof(float) * 3));

        // Unbind to prevent accidental modification.
        _vertexArray.Unbind();
        _vertexBuffer.Unbind();
        _elementArray.Unbind();
    }

    void OpenGlRenderer::CreateDebugGui()
    {
        ImGui_ImplSDL3_InitForOpenGL(_window, _context);
        ImGui_ImplOpenGL3_Init(OPEN_GL_VERSION);
    }
}
