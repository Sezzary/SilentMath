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
    {   /* Positions           Colors               Texture coords */
        -0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f, // Lower left corner
        -0.5f,  0.5f, 0.0f,    0.0f, 1.0f, 0.0f,    0.0f, 1.0f, // Upper left corner
         0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,    1.0f, 1.0f, // Upper right corner
         0.5f, -0.5f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f  // Lower right corner
    };

    static GLuint VERTEX_INDICES[] =
    {
        0, 2, 1, // Upper triangle
        0, 3, 2  // Lower triangle
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
        glClearColor(0.2, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        CreateShaderProgram();
        CreateDebugGui();

        _uniformId = glGetUniformLocation(_shader.Id, "scale");

        // Generate textures.
        stbi_set_flip_vertically_on_load(true);
        _imageBytes = stbi_load("Assets/pop_cat.png", &_imageRes.x, &_imageRes.y, &_imageColorChannelCount, 0);
        glGenTextures(1, &_textureId);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _textureId);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _imageRes.x, _imageRes.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, _imageBytes);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(_imageBytes);
        glBindTexture(GL_TEXTURE_2D, 0);

        _textureUniformId = glGetUniformLocation(_shader.Id, "tex0");
        _shader.Activate();
        glUniform1i(_textureUniformId, 0);
    }

    void OpenGlRenderer::Deinitialize()
    {
        // Delete objects.
        _vertexArray.Delete();
        _vertexBuffer.Delete();
        _elementArray.Delete();
        _shader.Delete();

        // Delete textures.
        glDeleteTextures(1, &_textureId);
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
        glClearColor(0.2, 0.2f, 0.2f, 1.0f);
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
        _shader.Activate();

        //glUniform1f(_uniformId, 0.5f);
        glBindTexture(GL_TEXTURE_2D, _textureId);

        _vertexArray.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
        _vertexArray.LinkAttrib(_vertexBuffer, 0, 3, GL_FLOAT, sizeof(float) * 8, (void*)0);
        _vertexArray.LinkAttrib(_vertexBuffer, 1, 3, GL_FLOAT, sizeof(float) * 8, (void*)(sizeof(float) * 3));
        _vertexArray.LinkAttrib(_vertexBuffer, 2, 2, GL_FLOAT, sizeof(float) * 8, (void*)(sizeof(float) * 6));

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
