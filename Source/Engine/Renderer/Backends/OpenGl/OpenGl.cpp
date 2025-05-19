#include "Framework.h"
#include "Engine/Renderer/Backends/OpenGl/OpenGl.h"

#include "Engine/Renderer/Base.h"

namespace Silent::Renderer
{
    // Vertex Shader source code
    static const char* VertexShaderSource = "#version 460 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

    //Fragment Shader source code
    static const char* FragmentShaderSource = "#version 460 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
    "}\n\0";

    float Vertices[] =
    {
        -0.5f, -0.5f * (float)sqrt(3) / 3, 0.0f,
        0.5f, -0.5f * (float)sqrt(3) / 3, 0.0f,
        0.0f, 0.5f * (float)sqrt(3) * 2 / 3, 0.0f
    };

    void OpenGlRenderer::Initialize(SDL_Window& window)
    {
        _window = &window;

        // Set OpenGL configuration.
        gladLoadGL();

        // Create OpenGL context.
        _glContext = SDL_GL_CreateContext(_window);
        if (!_glContext)
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

        int width  = 0;
        int height = 0;
        SDL_GetWindowSizeInPixels(_window, &width, &height);

        // Basic setup.
        glViewport(0, 0, width, height);
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.07f, 0.9f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if constexpr (IS_DEBUG)
        {
            InitializeDebugGui();
        }

        // Create shaders.
        
        glShaderSource(_vertexShader, 1, &VertexShaderSource, nullptr);
        glCompileShader(_vertexShader);

        glShaderSource(_fragmentShader, 1, &FragmentShaderSource, nullptr);
        glCompileShader(_fragmentShader);

        glAttachShader(_shaderProgram, _vertexShader);
        glAttachShader(_shaderProgram, _fragmentShader);
        glLinkProgram(_shaderProgram);

        glDeleteShader(_vertexShader);
        glDeleteShader(_fragmentShader);

        // Create vertex buffer.

        glGenVertexArrays(1, &_vao);
        glGenBuffers(1, &_vbo);

        glBindVertexArray(_vao);

        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void OpenGlRenderer::Deinitialize()
    {
        glDeleteVertexArrays(1, &_vao);
        glDeleteBuffers(1, &_vbo);
        glDeleteProgram(_shaderProgram);
    }

    void OpenGlRenderer::Update()
    {
        UpdateViewport();

        DrawFrame();
        DrawDebugGui();

        // Swap buffers.
        SDL_GL_SwapWindow(_window);
    }

    void OpenGlRenderer::SignalResize()
    {
        _isResized = true;
    }

    void OpenGlRenderer::SaveScreenshot() const
    {
        // TODO
    }

    void OpenGlRenderer::SubmitDebugGui(std::function<void()> drawFunc)
    {
        if constexpr (IS_DEBUG)
        {
            _debugGuiDrawCalls.push_back(drawFunc);
        }
    }

    void OpenGlRenderer::InitializeDebugGui()
    {
        constexpr char VERSION[] = "#version 460";

        ImGui::CreateContext();
        ImGui_ImplSDL3_InitForOpenGL(_window, _glContext);
        ImGui_ImplOpenGL3_Init(VERSION);
    }

    void OpenGlRenderer::UpdateViewport()
    {
        // Clear screen.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Resize viewport if window is resized.
        if (_isResized)
        {
            int width  = 0;
            int height = 0;
            SDL_GetWindowSizeInPixels(_window, &width, &height);
            glViewport(0, 0, width, height);
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
}
