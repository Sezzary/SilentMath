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
    ConfigurationManager& ApplicationManager::GetConfig()
    {
        return g_Config;
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
        g_Config.Initialize();

        // SDL.
        Log("Initializing SDL...");
        bool sdlStatus = SDL_Init(SDL_INIT_VIDEO);
        Assert(sdlStatus, "Failed to initialize SDL.");

        const auto& options = g_Config.GetOptions();

        // Create window.
        int flags = SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | (options.EnableFullscreen ? SDL_WINDOW_FULLSCREEN : 0);
        _window   = SDL_CreateWindow(WINDOW_NAME, options.WindowSize.x, options.WindowSize.y, flags);
        Assert(_window != nullptr, "Failed to create window.");

        // Input.
        Log("Initializing input...");
        _input.Initialize(g_Config.GetOptions());

        // Renderer.
        Log("Initializing renderer...");
        g_Renderer.Initialize(*_window);

        _isRunning = true;
        Log("Initialization complete.");
    }

    void ApplicationManager::Deinitialize()
    {
        // Input.
        Log("Deinitializing input...");
        _input.Deinitialize();

        // Renderer.
        Log("Deinitializing renderer...");
        g_Renderer.Deinitialize();

        // SDL.
        Log("Deinitializing SDL...");
        SDL_DestroyWindow(_window);
        SDL_Quit();

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
        _input.Update(*_window, g_Config.GetOptions(), _mouseWheelAxis);

        // TODO: Update game state here.

        HandleDebugMenu();
        UpdateDebug();

        // DEBUG

        if (_input.GetAction(In::A).IsHeld())
        {
            Log("Yeah");
        }
        else
        {
            Log("Nah");
        }
    }

    void ApplicationManager::Render()
    {
        // TODO: Can remove this for variable framerate?
        if (g_Time.GetTicks() <= 0)
        {
            return;
        }

        // Render scene.
        g_Renderer.Update();
    }

    void ApplicationManager::PollEvents()
    {
        while (SDL_PollEvent(&_event))
        {
            if constexpr (IS_DEBUG)
            {
                ImGui_ImplSDL3_ProcessEvent(&_event);
            }

            switch (_event.type)
            {
                case SDL_EVENT_QUIT:
                {
                    _isRunning = false;
                    break;
                }

                case SDL_EVENT_WINDOW_RESIZED:
                case SDL_EVENT_WINDOW_MINIMIZED:
                case SDL_EVENT_WINDOW_MAXIMIZED:
                case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
                case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
                {
                    auto& options = g_Config.GetOptions();

                    // Update window size in options.
                    auto res = Vector2i::Zero;
                    SDL_GetWindowSizeInPixels(_window, &res.x, &res.y);
                    g_Config.GetOptions().WindowSize = res;
                    g_Config.SaveOptions();

                    // Update framebuffer.
                    g_Renderer.SignalResizedFramebuffer();
                    break;
                }

                case SDL_EVENT_MOUSE_WHEEL:
                {
                    _mouseWheelAxis = Vector2(_event.wheel.x, _event.wheel.y);
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
