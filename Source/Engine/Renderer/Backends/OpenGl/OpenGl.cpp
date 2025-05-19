#include "Framework.h"
#include "Engine/Renderer/Backends/OpenGl/OpenGl.h"

#include "Engine/Application.h"
#include "Engine/Renderer/Base.h"

namespace Silent::Renderer
{
    // Vertex Shader source code
    static const char* VERTEX_SHADDER_SOURCE = "#version 460 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

    //Fragment Shader source code
    static const char* FRAGMENT_SHAER_SOURCE = "#version 460 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
    "}\n\0";

    static const float VERTICES[] =
    {
        -0.5f, -0.5f * (float)sqrt(3) / 3,     0.0f,
         0.5f, -0.5f * (float)sqrt(3) / 3,     0.0f,
         0.0f,  0.5f * (float)sqrt(3) * 2 / 3, 0.0f
    };

    void OpenGlRenderer::Initialize(SDL_Window& window)
    {
        _window = &window;

        // Set OpenGL configuration.
        gladLoadGL();

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

        CreateShaderProgram();
        CreateVertexBuffer();

        if constexpr (IS_DEBUG)
        {
            CreateDebugGui();
        }
    }

    void OpenGlRenderer::Deinitialize()
    {
        glDeleteVertexArrays(1, &_vao);
        glDeleteBuffers(1, &_vbo);
        glDeleteProgram(_shaderProgram);
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
        glUseProgram(_shaderProgram);
        glBindVertexArray(_vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
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

    void OpenGlRenderer::CreateShaderProgram()
    {
        glShaderSource(_vertexShader, 1, &VERTEX_SHADDER_SOURCE, nullptr);
        glCompileShader(_vertexShader);

        glShaderSource(_fragmentShader, 1, &FRAGMENT_SHAER_SOURCE, nullptr);
        glCompileShader(_fragmentShader);

        glAttachShader(_shaderProgram, _vertexShader);
        glAttachShader(_shaderProgram, _fragmentShader);
        glLinkProgram(_shaderProgram);

        glDeleteShader(_vertexShader);
        glDeleteShader(_fragmentShader);
    }

    void OpenGlRenderer::CreateVertexBuffer()
    {
        glGenVertexArrays(1, &_vao);
        glGenBuffers(1, &_vbo);

        glBindVertexArray(_vao);

        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void OpenGlRenderer::CreateDebugGui()
    {
        constexpr char VERSION[] = "#version 460";

        ImGui::CreateContext();
        ImGui_ImplSDL3_InitForOpenGL(_window, _context);
        ImGui_ImplOpenGL3_Init(VERSION);
    }
}
