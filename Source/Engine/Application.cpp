#include "Framework.h"
#include "Engine/Application.h"

#include "Engine/Input/Input.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Services/Filesystem.h"
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

    FilesystemManager& ApplicationManager::GetFilesystem()
    {
        return _work.Filesystem;
    }

    InputManager& ApplicationManager::GetInput()
    {
        return _work.Input;
    }

    OptionsManager& ApplicationManager::GetOptions()
    {
        return _work.Options;
    }

    RendererBase& ApplicationManager::GetRenderer()
    {
        return *_work.Renderer;
    }
    
    SavegameManager& ApplicationManager::GetSavegame()
    {
        return _work.Savegame;
    }

    TimeManager& ApplicationManager::GetTime()
    {
        return _work.Time;
    }

    Vector2i ApplicationManager::GetWindowResolution() const
    {
        auto res = Vector2i::Zero;
        SDL_GetWindowSizeInPixels(_window, &res.x, &res.y);
        return res;
    }

    void ApplicationManager::Initialize()
    {
        // Filesystem.
        _work.Filesystem.Initialize();

        // Debug.
        InitializeDebug();

        Log("Starting Silent Engine...");

        // Options.
        _work.Options.Load();

        // Parallelism.
        g_Parallel.Initialize();

        // Assets.
        _work.Assets.Initialize(_work.Filesystem.GetAssetsFolder() / "SILENT");

        // SDL.
        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            throw std::runtime_error("Failed to initialize SDL: " + std::string(SDL_GetError()));
        }

        // Collect window flags.
        int rendererFlag   = SDL_WINDOW_OPENGL;
        int fullscreenFlag = _work.Options->EnableFullscreen ? SDL_WINDOW_FULLSCREEN : 0;
        int maximizedFlag  = _work.Options->EnableMaximized  ? SDL_WINDOW_MAXIMIZED  : 0;
        int flags          = SDL_WINDOW_RESIZABLE | rendererFlag | fullscreenFlag | maximizedFlag;

        // Create window.
        _window = SDL_CreateWindow(APP_NAME, _work.Options->WindowedSize.x, _work.Options->WindowedSize.y, flags);
        if (_window == nullptr)
        {
            throw std::runtime_error("Failed to create window: " + std::string(SDL_GetError()));
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
        _work.Time.Initialize();

        while (_isRunning)
        {
            _work.Time.Update();

            Update();
            Render();

            _work.Time.WaitForNextTick();
        }
    }

    void ApplicationManager::ToggleFullscreen()
    {
        if (!SDL_SetWindowFullscreen(_window, !_work.Options->EnableFullscreen))
        {
            Log("Failed to toggle fullscreen mode: " + std::string(SDL_GetError()), LogLevel::Warning);
        }
    }

    void ApplicationManager::ToggleCursor()
    {
        // Show.
        if ((!_work.Options->EnableFullscreen || _work.Options->EnableDebugGui) && !SDL_CursorVisible())
        {
            if (!SDL_ShowCursor())
            {
                Log("Failed to show cursor: " + std::string(SDL_GetError()));
            }

            // Move cursor to window center.
            auto pos = GetWindowResolution().ToVector2() / 2.0f;
            SDL_WarpMouseInWindow(_window, pos.x, pos.y);
        }
        // Hide.
        else if (_work.Options->EnableFullscreen && SDL_CursorVisible())
        {
            if (!SDL_HideCursor())
            {
                Log("Failed to hide cursor: " + std::string(SDL_GetError()));
            }
        }
    }

    void ApplicationManager::Update()
    {
        PollEvents();
        _work.Input.Update(*_window, _mouseWheelAxis);

        // TODO: Update game state here.

        UpdateDebug();
    }

    void ApplicationManager::Render()
    {
        if (_work.Time.GetTicks() <= 0)
        {
            return;
        }

        // Render scene.
        _work.Renderer->Update();
    }

    void ApplicationManager::PollEvents()
    {
        auto event = SDL_Event{};
        while (SDL_PollEvent(&event))
        {
            if (_work.Options->EnableDebugGui)
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

                    auto res = Vector2i::Zero;
                    SDL_GetWindowSizeInPixels(_window, &res.x, &res.y);
                    if (_work.Options->WindowedSize == res)
                    {
                        break;
                    }

                    // Update options.
                    _work.Options->WindowedSize = res;
                    _work.Options.Save();

                    // Update window state.
                    _work.Renderer->SignalResize();
                    break;
                }

                case SDL_EVENT_WINDOW_MAXIMIZED:
                case SDL_EVENT_WINDOW_RESTORED:
                {
                    // Update options.
                    auto windowFlags               = SDL_GetWindowFlags(_window);
                    _work.Options->EnableMaximized = windowFlags & SDL_WINDOW_MAXIMIZED;
                    _work.Options.Save();

                    // Update window state.
                    _work.Renderer->SignalResize();
                    break;
                }

                case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
                case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
                {
                    bool isFullscreen = event.type == SDL_EVENT_WINDOW_ENTER_FULLSCREEN;

                    // Update options.
                    _work.Options->EnableFullscreen = isFullscreen;
                    _work.Options.Save();

                    // Show/hide cursor.
                    ToggleCursor();
                    
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
