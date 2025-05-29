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

    AssetManager& ApplicationManager::GetAssets()
    {
        return _work.Assets;
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

    RendererBase& ApplicationManager::GetRenderer()
    {
        return *_work.Renderer;
    }
    
    void ApplicationManager::Initialize()
    {
        _work.Config.Initialize();
        InitializeDebug();

        Log("Starting Silent Engine...");

        // Options.
        _work.Config.LoadOptions();

        // Parallelism.
        g_Parallel.Initialize();

        // Assets.
        _work.Assets.Initialize(_work.Config.GetAssetsFolder() / "SILENT");

        // SDL.
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
        _window = SDL_CreateWindow(APP_NAME, options.WindowedSize.x, options.WindowedSize.y, flags);
        if (_window == nullptr)
        {
            throw std::runtime_error("Failed to create window.");
        }

        // Renderer.
        _work.Renderer = CreateRenderer(RendererType::OpenGl);
        if (_work.Renderer == nullptr)
        {
            throw std::runtime_error("Failed to create renderer.");
        }
        _work.Renderer->Initialize(*_window);

        // Input.
        _work.Input.Initialize();

        // Finish.
        Log("Startup complete.");
        _isRunning = true;
    }

    void ApplicationManager::Deinitialize()
    {
        Log("Shutting down Silent Engine...");

        // Input.
        _work.Input.Deinitialize();

        // Renderer.
        _work.Renderer->Deinitialize();

        // Parallelism.
        g_Parallel.Deinitialize();

        // SDL.
        SDL_DestroyWindow(_window);
        SDL_Quit();

        // Finish.
        Log("Shutdown complete.");
    }

    void ApplicationManager::Run()
    {
        g_Time.Initialize();

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

    void ApplicationManager::Update()
    {
        PollEvents();

        // Update input state.
        _work.Input.Update(*_window, _mouseWheelAxis);

        // TODO: Update game state here.

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
            if (options.EnableDebugGui)
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

                    bool isFullscreen = event.type == SDL_EVENT_WINDOW_ENTER_FULLSCREEN;

                    // Update cursor.
                    if (isFullscreen)
                    {
                        // Hide cursor.
                        if (!SDL_HideCursor())
                        {
                            Log("Failed to hide cursor: " + (std::string(SDL_GetError())));
                        }
                    }
                    else
                    {
                        // Show cursor.
                        if (!SDL_ShowCursor())
                        {
                            Log("Failed to show cursor: " + (std::string(SDL_GetError())));
                        }

                        // TODO: Warps to corner instead.
                        // Move cursor to window center.
                        auto res = Vector2i::Zero;
                        SDL_WarpMouseInWindow(_window, res.x / 2, res.y / 2);
                    }
                    
                    // Update options.
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
