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

    ConfigurationManager& ApplicationManager::GetConfig()
    {
        return _config;
    }

    SavegameManager& ApplicationManager::GetSavegame()
    {
        return _savegame;
    }

    InputManager& ApplicationManager::GetInput()
    {
        return _input;
    }

    void ApplicationManager::Initialize()
    {
        Log("Starting Silent Engine.");
        
        // Debug.
        InitializeDebug();

        // Configuration.
        Log("Initializing configuration...");
        _config.Initialize();

        // SDL.
        Log("Initializing SDL...");
        bool sdlStatus = SDL_Init(SDL_INIT_VIDEO);
        Assert(sdlStatus, "Failed to initialize SDL.");

        const auto& options = _config.GetOptions();

        // Collect window flags.
        int rendererFlag   = SDL_WINDOW_VULKAN;
        int fullscreenFlag = options.EnableFullscreen ? SDL_WINDOW_FULLSCREEN : 0;
        int maximizedFlag  = options.EnableMaximized  ? SDL_WINDOW_MAXIMIZED  : 0;
        int flags          = SDL_WINDOW_RESIZABLE | rendererFlag | fullscreenFlag | maximizedFlag;

        // Create window.
        _window = SDL_CreateWindow(WINDOW_NAME, options.WindowedSize.x, options.WindowedSize.y, flags);
        Assert(_window != nullptr, "Failed to create window.");

        // Renderer.
        Log("Initializing renderer...");
        g_Renderer = CreateRenderer(RendererType::Vulkan);
        Assert(g_Renderer != nullptr, "Failed to create renderer.");
        g_Renderer->Initialize(*_window);

        // Input.
        Log("Initializing input...");
        _input.Initialize();

        // Finish.
        Log("Initialization complete.");
        _isRunning = true;
    }

    void ApplicationManager::Deinitialize()
    {
        // Input.
        Log("Deinitializing input...");
        _input.Deinitialize();

        // Renderer.
        Log("Deinitializing renderer...");
        g_Renderer->Deinitialize();

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
        auto& options = _config.GetOptions();

        if (SDL_SetWindowFullscreen(_window, !options.EnableFullscreen))
        {
            Log("Failed to toggle fullscreen mode.", LogLevel::Warning);
            return;
        }

        options.EnableFullscreen = !options.EnableFullscreen;
        _config.SaveOptions();
    }

    void ApplicationManager::ToggleDebugMenu()
    {
        _showDebugMenu = !_showDebugMenu;
    }

    void ApplicationManager::HandleDebugMenu()
    {
        if constexpr (IS_DEBUG)
        {
            if (!_showDebugMenu)
            {
                return;
            }

            CreateGui([]()
                {
                    ImGui::Begin("My Window");
                    ImGui::Text("Hello. It's me. =^.^=");
                    ImGui::End();
                });
        }
    }

    void ApplicationManager::Update()
    {
        PollEvents();

        // Update input state.
        _input.Update(*_window, _mouseWheelAxis);

        // TODO: Update game state here.

        HandleDebugMenu();
        UpdateDebug();
    }

    void ApplicationManager::Render()
    {
        if (g_Time.GetTicks() <= 0)
        {
            return;
        }

        // Render scene.
        g_Renderer->Update();
    }

    void ApplicationManager::PollEvents()
    {
        auto event = SDL_Event{};
        while (SDL_PollEvent(&event))
        {
            if constexpr (IS_DEBUG)
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

                    auto& options = _config.GetOptions();
                    
                    auto res = Vector2i::Zero;
                    SDL_GetWindowSizeInPixels(_window, &res.x, &res.y);
                    if (options.WindowedSize == res)
                    {
                        break;
                    }

                    // Update options.
                    options.WindowedSize = res;
                    _config.SaveOptions();

                    // Update window state.
                    g_Renderer->SignalResizedFramebuffer();
                    break;
                }

                case SDL_EVENT_WINDOW_MAXIMIZED:
                case SDL_EVENT_WINDOW_RESTORED:
                {
                    auto& options = _config.GetOptions();

                    // Update options.
                    auto windowFlags        = SDL_GetWindowFlags(_window);
                    options.EnableMaximized = windowFlags & SDL_WINDOW_MAXIMIZED;
                    _config.SaveOptions();

                    // Update window state.
                    g_Renderer->SignalResizedFramebuffer();
                    break;
                }

                case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
                case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
                {
                    auto& options = _config.GetOptions();

                    // Update options.
                    bool isFullscreen        = event.type == SDL_EVENT_WINDOW_ENTER_FULLSCREEN;
                    options.EnableFullscreen = isFullscreen;
                    _config.SaveOptions();

                    // Update window state.
                    SDL_SetWindowFullscreen(_window, isFullscreen);
                    g_Renderer->SignalResizedFramebuffer();
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
