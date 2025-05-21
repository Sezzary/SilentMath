#include "Framework.h"
#include "Engine/Application.h"

#include "Engine/Input/Input.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Services/Configuration.h"
#include "Engine/Services/Time.h"
#include "Utils/Parallel.h"

using namespace Silent::Input;
using namespace Silent::Renderer;
using namespace Silent::Services;
using namespace Silent::Utils;

namespace Silent
{
    ApplicationManager g_App = ApplicationManager();

    RendererBase& ApplicationManager::GetRenderer()
    {
        return *_work.Renderer;
    }

    InputManager& ApplicationManager::GetInput()
    {
        return _work.Input;
    }

    SavegameManager& ApplicationManager::GetSavegame()
    {
        return _work.Savegame;
    }

    ConfigurationManager& ApplicationManager::GetConfig()
    {
        return _work.Config;
    }

    DebugPage ApplicationManager::GetDebugPage()
    {
        return _debugPage;
    }

    void ApplicationManager::SetDebugPage(DebugPage page)
    {
        _debugPage = page;
    }

    void ApplicationManager::Initialize()
    {
        Log("Starting Silent Engine.");
        
        // Debug.
        InitializeDebug();

        // Configuration.
        Log("Initializing configuration...");
        _work.Config.Initialize();

        // SDL.
        Log("Initializing SDL...");
        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            throw std::runtime_error("Failed to initialize SDL.");
        }

        const auto& options = _work.Config.GetOptions();

        // Collect window flags.
        int rendererFlag   = SDL_WINDOW_OPENGL;
        int fullscreenFlag = options.EnableFullscreen ? SDL_WINDOW_FULLSCREEN : 0;
        int maximizedFlag  = options.EnableMaximized  ? SDL_WINDOW_MAXIMIZED  : 0;
        int flags          = SDL_WINDOW_RESIZABLE | rendererFlag | fullscreenFlag | maximizedFlag;

        // Create window.
        _window = SDL_CreateWindow(WINDOW_NAME, options.WindowedSize.x, options.WindowedSize.y, flags);
        Assert(_window != nullptr, "Failed to create window.");

        // Renderer.
        Log("Initializing renderer...");
        _work.Renderer = CreateRenderer(RendererType::OpenGl);
        if (_work.Renderer == nullptr)
        {
            throw std::runtime_error("Failed to create renderer.");
        }
        _work.Renderer->Initialize(*_window);

        // Input.
        Log("Initializing input...");
        _work.Input.Initialize();

        // Finish.
        Log("Initialization complete.");
        _isRunning = true;
    }

    void ApplicationManager::Deinitialize()
    {
        Log("Exiting Silent Engine.");

        // Input.
        Log("Deinitializing input...");
        _work.Input.Deinitialize();

        // Renderer.
        Log("Deinitializing renderer...");
        _work.Renderer->Deinitialize();

        // SDL.
        Log("Deinitializing SDL...");
        SDL_DestroyWindow(_window);
        SDL_Quit();

        // Finish.
        Log("Deinitialization complete.");
    }

    void ApplicationManager::Run()
    {
        g_Time.Reset();

        while (_isRunning)
        {
            g_Time.Update();

            Update();
            Render();

            g_Time.WaitForNextTick();
        }
    }

    void ApplicationManager::ToggleFullscreen()
    {
        auto& options = _work.Config.GetOptions();

        if (!SDL_SetWindowFullscreen(_window, !options.EnableFullscreen))
        {
            Log("Failed to toggle fullscreen mode.", LogLevel::Warning);
        }
    }

    void ApplicationManager::ToggleDebugGui()
    {
        _enableDebugGui = !_enableDebugGui;
    }

    void ApplicationManager::HandleDebugGui()
    {
        const auto& options = _work.Config.GetOptions();
        if (!options.EnableDebugMode || !_enableDebugGui)
        {
            return;
        }

        // DEMO
        CreateGui([]()
            {
                ImGui::ShowDemoWindow();
            });

        CreateGui([]()
            {
                ImGui::Begin("My Window");
                ImGui::Text("Hello. It's me. =^.^=");
                ImGui::End();
            });
    }

    void ApplicationManager::Update()
    {
        PollEvents();

        // Update input state.
        _work.Input.Update(*_window, _mouseWheelAxis);

        // TODO: Update game state here.

        HandleDebugGui();
        UpdateDebug();
    }

    void ApplicationManager::Render()
    {
        if (g_Time.GetTicks() <= 0)
        {
            return;
        }

        // Render scene.
        _work.Renderer->Update();
    }

    void ApplicationManager::PollEvents()
    {
        const auto& options = _work.Config.GetOptions();

        auto event = SDL_Event{};
        while (SDL_PollEvent(&event))
        {
            if (options.EnableDebugMode && _enableDebugGui)
            {
                ImGui_ImplSDL3_ProcessEvent(&event);
            }

            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                {
                    _isRunning = false;
                    break;
                }

                case SDL_EVENT_WINDOW_RESIZED:
                {
                    auto windowFlags = SDL_GetWindowFlags(_window);
                    if (windowFlags & SDL_WINDOW_FULLSCREEN ||
                        windowFlags & SDL_WINDOW_MAXIMIZED)
                    {
                        break;
                    }

                    auto& options = _work.Config.GetOptions();
                    
                    auto res = Vector2i::Zero;
                    SDL_GetWindowSizeInPixels(_window, &res.x, &res.y);
                    if (options.WindowedSize == res)
                    {
                        break;
                    }

                    // Update options.
                    options.WindowedSize = res;
                    _work.Config.SaveOptions();

                    // Update window state.
                    _work.Renderer->SignalResize();
                    break;
                }

                case SDL_EVENT_WINDOW_MAXIMIZED:
                case SDL_EVENT_WINDOW_RESTORED:
                {
                    auto& options = _work.Config.GetOptions();

                    // Update options.
                    auto windowFlags        = SDL_GetWindowFlags(_window);
                    options.EnableMaximized = windowFlags & SDL_WINDOW_MAXIMIZED;
                    _work.Config.SaveOptions();

                    // Update window state.
                    _work.Renderer->SignalResize();
                    break;
                }

                case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
                case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
                {
                    auto& options = _work.Config.GetOptions();

                    // Update options.
                    bool isFullscreen        = event.type == SDL_EVENT_WINDOW_ENTER_FULLSCREEN;
                    options.EnableFullscreen = isFullscreen;
                    _work.Config.SaveOptions();

                    // Update window state.
                    _work.Renderer->SignalResize();
                    break;
                }

                case SDL_EVENT_MOUSE_WHEEL:
                {
                    _mouseWheelAxis = Vector2(event.wheel.x, event.wheel.y);
                    break;
                }

                default:
                {
                    break;
                }
            }
        }
    }
}
