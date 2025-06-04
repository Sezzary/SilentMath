#include "Framework.h"
#include "Engine/Renderer/Backends/OpenGl/OpenGl.h"

#include "Engine/Application.h"
#include "Engine/Renderer/Backends/OpenGl/ElementArrayBuffer.h"
#include "Engine/Renderer/Backends/OpenGl/Shaders.h"
#include "Engine/Renderer/Backends/OpenGl/Texture.h"
#include "Engine/Renderer/Backends/OpenGl/VertexArray.h"
#include "Engine/Renderer/Backends/OpenGl/VertexBuffer.h"
#include "Engine/Renderer/Backends/OpenGl/View.h"
#include "Engine/Renderer/Base.h"
#include "Engine/Services/Configuration.h"
#include "Engine/Services/Time.h"
#include "Utils/Utils.h"

using namespace Silent::Services;
using namespace Silent::Utils;

namespace Silent::Renderer
{
    static float VERTICES[] =
    {/*  Positions              Colors                  Texture coords */
        -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,    0.0f, 0.0f,
        -0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,    5.0f, 0.0f,
         0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,    0.0f, 0.0f,
         0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,    5.0f, 0.0f,
         0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,    2.5f, 5.0f
    };

    static uint VERTEX_INDICES[] =
    {
        0, 1, 2,
        0, 2, 3,
        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4
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

        // Get screen resolution.
        auto res = GetScreenResolution();

        // Viewport setup.
        glViewport(0, 0, res.x, res.y);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        CreateShaderProgram();
        CreateDebugGui();

        // Texture setup.
        _popCat = Texture("Assets/pop_cat.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
        _popCat.TextureUnit(_shaders, "tex0", 0);

        // View setup.
        _view = View(Vector3(0.0f, 0.0f, 2.0f), res);
    }

    void OpenGlRenderer::Deinitialize()
    {
        // Delete objects.
        _vertexArray.Delete();
        _vertexBuffer.Delete();
        _elementArray.Delete();
        _shaders.Delete();

        _popCat.Delete();
    }

    void OpenGlRenderer::Update()
    {
        // Update viewport.
        UpdateViewport();

        // Render.
        DrawFrame();
        //DrawGui();
        DrawDebugGui();

        // Swap buffers.
        if (!SDL_GL_SwapWindow(_window))
        {
            Log("Failed to swap render buffer: " + std::string(SDL_GetError()), LogLevel::Warning);
        }

        // Clear scene. TODO
    }

    void OpenGlRenderer::SaveScreenshot() const
    {
        constexpr uint COLOR_CHANNEL_COUNT = 3; // RGB.

        const auto& config = g_App.GetConfig();

        // Get window size.
        auto res = GetScreenResolution();

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
        if (!stbi_write_png(path.string().c_str(), res.x, res.y, COLOR_CHANNEL_COUNT, pixels.data(), res.x * COLOR_CHANNEL_COUNT))
        {
            Log("Failed to save screenshot.", LogLevel::Warning, LogMode::DebugRelease, true);
        }
    }

    void OpenGlRenderer::UpdateViewport()
    {
        // Resize viewport if window is resized.
        if (_isResized)
        {
            _isResized = false;

            auto res = GetScreenResolution();
            glViewport(0, 0, res.x, res.y);
        }

        // Clear screen.
        glClearColor(0.3f, 0.5f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGlRenderer::DrawFrame()
    {
        _shaders.Activate("Default");

        _view.Move();
        _view.ExportMatrix(glm::radians(45.0f), 0.1f, 100.0f, _shaders, "camMat");

        _popCat.Bind();

        _vertexArray.Bind();
        glDrawElements(GL_TRIANGLES, sizeof(VERTEX_INDICES) / sizeof(int), GL_UNSIGNED_INT, 0);
    }

    float quadVertices[] =
    {
        /*150 - 130, 20 - 62,
        150 - 130, 20 - 43,
        150 - 11,  20 - 62,
        150 - 11,  20 - 43*/

        
        /*-106, -88, -106, -92, -146, -88, -150, -92,
        -106, -87, -106, -83, -146, -87, -142, -83
        
        
        -42, -88, -42, -92, -2, -88, 2, -92,
        -42, -87, -42, -83, -2, -87, -6, -83
        
        
        -146, -88, -150, -92, -146, 42, -150, 46,
        -145, -87, -141, -83, -145, 41, -141, 37
        
        
        -2, -88, 2, -92, -2, 42, 2, 46,
        -3, -87, -7, -83, -3, 41, -7, 37*/
        
       // Scroll bar arrows.
       /*4, -1, -1, 7,  8, 7,
       5, 1,  0, 8,  9, 8,
       4, 96,  0, 88,  8, 88,
       5, 97,  1, 89,  9, 89*/
       
       // Scroll bar track
        /*0, 0 , 0, 96 , 4, 0, 4, 96,
        8, 0 , 8, 96 , 4, 0 , 4, 96 */

        // Slot border
        /*-107, -88 , -146, -88,
         -42, -88 , -2, -88 ,
         -146, -88,  -146, 42 ,
         -2,   -88,  -2, 42 ,
         -146, 42 , -2, 42 */

         // SLot box
        /*-139, -81,
        -139,  37,
        -9,   -81,
        -9,    37*/
        
         // SAvegame entry border
        -131, -62, -11, -62 ,
        -131, -43, -11, -43 ,
        -131, -62, -131, -44,
        -11,  -62, -11, -44 

        // Now checking MEMORY CARD border
        /*-144, -36,  -4, -36,
        -144, 2,  -4, 2,
        -144, -36, -144, 2,
        -4, -36,  -4, 2,*/
        // glow frame quads
        /*-144, -36 , -148, -40 ,-4, -36 , 0, -40,
        -144, 2 ,  -148, 6 , -4, 2 , 0, 6 ,
        -144, -36 ,  -148, -40 , -144, 2 , -148, 6 ,
        -4, -36 ,  0, -40 ,  -4, 2 ,  0, 6 */
        // some coloured line
       /* -142, -33 , 136, 33,
        -142, -33 , 136, 33,*/
        

         //-144, -36 ,  -148, -40 ,  -4, -36 ,  0, -40 
        // x, y
        /*0.0f, 0.0f,
        0.0f, 120.0f,
        160.0f, 120.0f,
        160.0f, 0.0f*/
    };

    void OpenGlRenderer::DrawGui()
    {
        static unsigned int vao, vbo;
        static bool isFirstTime = true;
        if (isFirstTime)
        {
            _shaders.Initialize("Quad");

            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);

            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
            glEnableVertexAttribArray(0);

            isFirstTime = false;
        }

        glUseProgram(_shaders._programIds.at("Quad"));
        glUniform3f(glGetUniformLocation(_shaders._programIds.at("Quad"), "uColor"), 0.63f, 0.63f, 0.63f);

        // Set line width to match PSX resolution.
        auto ratio = GetScreenResolution().ToVector2() / Vector2(320.0f, 240.0f);
        int  width = std::max((int)round(std::min(ratio.x, ratio.y)), 1);
        glLineWidth(width);

        glBindVertexArray(vao);
        for (int i = 0; i < 4; i++)
        {
            glDrawArrays(GL_LINES, i * 2, 2);
            //glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4); // Draw each quad (4 vertices at a time)
        }
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
        // Generate shader program.
        _shaders.Initialize("Default");

        // Generate and bind vertex array object.
        _vertexArray.Initialize();
        _vertexArray.Bind();

        // Generate vertex buffer and element array objects.
        _vertexBuffer.Initialize(ToSpan(VERTICES));
        _elementArray.Initialize(ToSpan(VERTEX_INDICES));

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
