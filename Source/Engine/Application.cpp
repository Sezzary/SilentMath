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

        // Initialize configuration.
        Log("Initializing configuration...");
        g_Config.Initialize();

        // Initialize SDL.
        Log("Initializing SDL...");
        bool sdlStatus = SDL_Init(SDL_INIT_VIDEO);
        Assert(sdlStatus, "Failed to initialize SDL.");

        const auto& options = g_Config.GetOptions();

        // Create window.
        int flags = SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | (options.EnableFullscreen ? SDL_WINDOW_FULLSCREEN : 0);
        _window   = SDL_CreateWindow(WINDOW_NAME, options.WindowSize.x, options.WindowSize.y, flags);
        Assert(_window != nullptr, "Failed to create window.");

        // Initialize input.
        Log("Initializing input...");
        _input.Initialize(g_Config.GetOptions());

        // Initialize renderer.
        Log("Initializing renderer...");
        g_Renderer.Initialize(*_window);

        _isRunning = true;
        Log("Initialization complete.");
    }

    void ApplicationManager::Deinitialize()
    {
        // Deinitialize input.
        Log("Deinitializing input...");
        _input.Deinitialize();

        // Deinitialize renderer.
        Log("Deinitializing renderer...");
        g_Renderer.Deinitialize();

        // Deinitialize SDL.
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

    void ApplicationManager::Update()
    {
        PollEvents();

        // Update input state.
        _input.Update(*_window, g_Config.GetOptions(), _mouseWheelAxis);

        // TODO: Update game state here.

        // DEBUG

        if (_input.GetAction(In::A).IsHeld())
        {
            Log("y");
        }
        else
        {
            Log("n");
        }

        CreateGui([]()
            {
                ImGui::Begin("My Window");
                ImGui::Text("Hello!");
                ImGui::End();
            });
    }

    void ApplicationManager::Render()
    {
        if (g_Time.GetTicks() <= 0)
        {
            return;
        }

        // Wait for previous frame to finish rendering.
        static auto prevFrameFut = std::future<void>();
        if (prevFrameFut.valid())
        {
            prevFrameFut.wait();
        }

        // TODO: Parallelism requires storing a separate render buffer for game data
        // so that it doesn't get overwritten by `ApplicationManager::Update` while 
        // `g_Renderer.Update` runs in the background.

        // Render scene.
        g_Renderer.Update();
        //prevFrameFut = g_Parallel.AddTask([this]() { g_Renderer.Update(); });
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

                    // Update options.
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
