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
    void ApplicationManager::Initialize()
    {
        Log("Starting Silent Engine.");

        // Initialize SDL.
        Log("Initializing SDL...");
        bool sdlStatus = SDL_Init(SDL_INIT_VIDEO);
        Assert(sdlStatus, "Failed to initialize SDL.");

        // Create window.
        _window = SDL_CreateWindow(WINDOW_NAME, 800, 600, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
        Assert(_window != nullptr, "Failed to create window.");

        // Initialize configuration.
        Log("Initializing configuration...");
        _config.Initialize();

        // Initialize input.
        Log("Initializing input...");
        g_Input.Initialize(_config.GetSettings());

        // Initialize renderer.
        Log("Initializing renderer...");
        _renderer.Initialize(*_window);

        _isRunning = true;
        Log("Initialization complete.");
    }

    void ApplicationManager::Deinitialize()
    {
        // Deinitialize input.
        Log("Deinitializing input...");
        g_Input.Deinitialize();

        // Deinitialize renderer.
        Log("Deinitializing renderer...");
        _renderer.Deinitialize();

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
        auto mouseWheelAxis = Vector2::Zero;

        // Poll events.
        while (SDL_PollEvent(&_event))
        {
            if constexpr (IS_DEBUG)
            {
                // Capture events for ImGui.
                ImGui_ImplSDL3_ProcessEvent(&_event);
            }

            // Handle each event.
            switch (_event.type)
            {
                case SDL_EVENT_QUIT:
                {
                    _isRunning = false;
                    break;
                }

                case SDL_EVENT_MOUSE_WHEEL:
                {
                    mouseWheelAxis = Vector2(_event.wheel.x, _event.wheel.y);
                    break;
                }

                default:
                    break;
            }
        }

        // Update input state.
        g_Input.Update(*_window, _config.GetSettings(), mouseWheelAxis);

        // TODO: Update game state here.

        // DEBUG

        if (g_Input.GetAction(In::A).IsReleased())
        {
            Log("y");
        }
        else
        {
            Log("n");
        }

        _renderer.SubmitGui(
            []()
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
        // `_renderer.Update` runs in the background.

        // Render scene.
        _renderer.Update();
        //prevFrameFut = g_Parallel.AddTask([this]() { _renderer.Update(); });
    }
}
