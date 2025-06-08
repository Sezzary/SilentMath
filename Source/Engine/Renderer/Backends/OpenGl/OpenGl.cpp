#include "Framework.h"
#include "Engine/Renderer/Backends/OpenGl/OpenGl.h"

#include "Engine/Application.h"
#include "Engine/Assets/Assets.h"
#include "Engine/Renderer/Backends/OpenGl/ElementBuffer.h"
#include "Engine/Renderer/Backends/OpenGl/ShaderProgram.h"
#include "Engine/Renderer/Backends/OpenGl/Texture.h"
#include "Engine/Renderer/Backends/OpenGl/VertexArray.h"
#include "Engine/Renderer/Backends/OpenGl/VertexBuffer.h"
#include "Engine/Renderer/Backends/OpenGl/View.h"
#include "Engine/Renderer/Base.h"
#include "Engine/Services/Configuration.h"
#include "Engine/Services/Time.h"
#include "Utils/Utils.h"

using namespace Silent::Assets;
using namespace Silent::Services;
using namespace Silent::Utils;

namespace Silent::Renderer
{
    static auto TRI_POSITIONS = std::vector<float>
    {
        -0.5f,  0.0f, 0.0f, // Left.
         0.5f,  0.5f, 0.0f, // Bottom.
         0.5f, -0.5f, 0.0f  // Top.
    };
    static auto TRI_COLORS = std::vector<float>
    {
        0.0f, 0.0f, 0.95f, // Left.
        0.0f, 0.0f, 0.95f, // Bottom.
        0.0f, 0.0f, 0.95f  // Top.
    };
    static auto TRI_TEXTURE_COORDS = std::vector<float>
    {
        1.0f, 1.0f, // Top-right.
        1.0f, 0.0f, // Bottom-right.
        0.0f, 0.0f, // Bottom-left.
        0.0f, 1.0f  // Top-left.
    };
    static auto TRI_INDICES = std::vector<uint>
    {
        0, 1, 2
    };

    static auto QUAD_POSITIONS = std::vector<float>
    {
         0.5f,  0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };
    static auto QUAD_COLORS = std::vector<float>
    {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f
    };
    static auto QUAD_TEXTURE_COORDS = std::vector<float>
    {
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f
    };
    static auto QUAD_INDICES = std::vector<uint>
    {
        0, 1, 3,
        1, 2, 3
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

        // Log renderer specs.
        Log("Using OpenGL renderer:");

        auto version = std::string((const char*)glGetString(GL_VERSION));
        Log("    Version: " + version + ".");

        auto shadingLangVersion = std::string((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
        Log("    Shading language version: " + shadingLangVersion + ".");

        auto gpu    = std::string((const char*)glGetString(GL_RENDERER));
        auto vendor = std::string((const char*)glGetString(GL_VENDOR));
        Log("    GPU: " + gpu + ", " + vendor + ".");

        int attribCountMax = 0;
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &attribCountMax);
        Log("    " + std::to_string(attribCountMax) + " vertex attributes available.", LogLevel::Info, LogMode::Debug);

        int varyingVarCountMax = 0;
        glGetIntegerv(GL_MAX_VARYING_VECTORS, &varyingVarCountMax);
        Log("    " + std::to_string(varyingVarCountMax) + " varying variables available.", LogLevel::Info, LogMode::Debug);

        int combinedTexUnitCountMax = 0;
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &combinedTexUnitCountMax);
        Log("    " + std::to_string(combinedTexUnitCountMax) + " combined texture image units available.", LogLevel::Info, LogMode::Debug);

        int vertTexImageUnitCountMax = 0;
        glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &vertTexImageUnitCountMax);
        Log("    " + std::to_string(vertTexImageUnitCountMax) + " vertex texture image units available.", LogLevel::Info, LogMode::Debug);

        int texSizeMax = 0;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texSizeMax);
        Log("    " + std::to_string(texSizeMax)  + " max texture size.", LogLevel::Info, LogMode::Debug);

        int uniBlockSizeMax = 0;
        glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &uniBlockSizeMax);
        Log("    " + std::to_string(uniBlockSizeMax) + " max uniform block size.", LogLevel::Info, LogMode::Debug);

        int renderBufferSizeMax = 0;
        glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &renderBufferSizeMax);
        Log("    " + std::to_string(renderBufferSizeMax) + " max renderbuffer size.", LogLevel::Info, LogMode::Debug);
    }

    void OpenGlRenderer::Deinitialize()
    {
        for (auto& [keyName, shaderProg] : _shaderPrograms)
        {
            shaderProg.Delete();
        }

        // Delete objects.
        _vertexArray.Delete();
        _vertexPositionBuffer.Delete();
        _vertexColorBuffer.Delete();
        _vertexTexCoordBuffer.Delete();
        _elementBuffer.Delete();
        _texture0.Delete();
        _texture1.Delete();
    }

    void OpenGlRenderer::Update()
    {
        _drawCallCount = 0;

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

    void OpenGlRenderer::RefreshTextureFilter()
    {
        _texture0.RefreshFilter();
        _texture1.RefreshFilter();
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
        // Set wireframe mode.
        if (g_DebugData.EnableWireframeMode)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

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

    static void DoCoolColorThing()
    {
        static int timer      = 0;
        int        timerCount = timer & 0x7F;
        
        // Fade start color.
        int colorStart = 0;
        if (timerCount >= 32)
        {
            colorStart = 32;
            if (timerCount < 64)
            {
                colorStart = 32;
            }
            else if (timerCount < 96)
            {
                colorStart = 96 - timerCount;
            }
            else
            {
                colorStart = 0;
            }
        }
        else
        {
            colorStart = timerCount;
        }

        // Fade end color.
        int colorEnd = 0;
        if (timerCount >= 32)
        {
            if (timerCount < 64)
            {
                colorEnd = timerCount - 32;
            }
            else if (timerCount >= 96)
            {
                colorEnd = 128 - timerCount;
            }
            else
            {
                colorEnd = 32;
            }
        }
        else
        {
            colorEnd = 0;
        }

        timer++;

        TRI_COLORS[0] = 0.0f;
        TRI_COLORS[1] = colorEnd / 31.0f;
        TRI_COLORS[2] = 1.0f;

        TRI_COLORS[3]  = 0.0f;
        TRI_COLORS[4] = colorStart / 31.0f;
        TRI_COLORS[5] = 1.0f;

        TRI_COLORS[6] = 0.0f;
        TRI_COLORS[7] = colorStart / 31.0f;
        TRI_COLORS[8] = 1.0f;
    }

    void OpenGlRenderer::DrawFrame()
    {
        DoCoolColorThing();

        static auto transform = Matrix::Identity;
        transform.Rotate(glm::radians(0.5f), Vector3::UnitZ);
        //transform = glm::scale(transform, glm::vec3(0.5f, 0.5f, 0.5f));

        auto& shaderProg = _shaderPrograms.at("Default");

        shaderProg.Activate();
        shaderProg.SetVec3("offset", Vector3(0.5f, 0.5f, 0.5f));
        shaderProg.SetFloat("blendAlpha", g_DebugData.BlendAlpha);
        shaderProg.SetMatrix("transform", transform);

        _vertexColorBuffer.Bind();
        glBufferData(GL_ARRAY_BUFFER, TRI_COLORS.size() * sizeof(float), TRI_COLORS.data(), GL_DYNAMIC_DRAW);

        _texture0.Bind();
        _texture1.Bind();
        _vertexArray.Bind();
        glDrawElements(GL_TRIANGLES, sizeof(TRI_INDICES) / sizeof(uint), GL_UNSIGNED_INT, 0);
        _drawCallCount++;
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
            _shaderPrograms.emplace("Quad", ShaderProgram("Quad"));

            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);

            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
            glEnableVertexAttribArray(0);

            isFirstTime = false;
        }

        _shaderPrograms.at("Quad").Activate();
        glUniform3f(glGetUniformLocation(_shaderPrograms.at("Quad").GetId(), "uColor"), 0.63f, 0.63f, 0.63f);

        // Set line width to match PSX resolution.
        auto  ratio = GetScreenResolution().ToVector2() / Vector2(320.0f, 240.0f);
        float width = std::max(round(std::min(ratio.x, ratio.y)), 1.0f);
        glLineWidth(width);

        glBindVertexArray(vao);
        for (int i = 0; i < 4; i++)
        {
            glDrawArrays(GL_LINES, i * 2, 2);
            //glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4); // Draw each quad (4 vertices at a time)

            _drawCallCount++;
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
        _shaderPrograms.emplace("Default", ShaderProgram("Default"));

        // Generate and bind vertex array object.
        _vertexArray.Initialize();
        _vertexArray.Bind();

        // Generate vertex buffer and element array objects.
        _vertexPositionBuffer.Initialize(ToSpan(TRI_POSITIONS));
        _vertexColorBuffer.Initialize(ToSpan(TRI_COLORS), GL_DYNAMIC_DRAW);
        _vertexTexCoordBuffer.Initialize(ToSpan(TRI_TEXTURE_COORDS));
        _elementBuffer.Initialize(ToSpan(TRI_INDICES));

        // Link attributes.
        _vertexArray.LinkAttrib(_vertexPositionBuffer, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
        _vertexArray.LinkAttrib(_vertexColorBuffer, 1, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
        _vertexArray.LinkAttrib(_vertexTexCoordBuffer, 2, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

        // Unbind to prevent accidental modification.
        _vertexArray.Unbind();
        _vertexPositionBuffer.Unbind();
        _vertexColorBuffer.Unbind();
        _vertexTexCoordBuffer.Unbind();
        _elementBuffer.Unbind();

        // Load textures.
        _texture0 = Texture("Assets/brick.png", GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
        _texture1 = Texture("Assets/SILENT/TIM/BG_ETC.TIM", GL_TEXTURE1, GL_RGBA, GL_UNSIGNED_BYTE);

        _shaderPrograms.at("Default").Activate();
        _shaderPrograms.at("Default").SetInt("tex0", 0);
        _shaderPrograms.at("Default").SetInt("tex1", 1);
    }

    void OpenGlRenderer::CreateDebugGui()
    {
        ImGui_ImplSDL3_InitForOpenGL(_window, _context);
        ImGui_ImplOpenGL3_Init(OPEN_GL_VERSION);
    }
}
