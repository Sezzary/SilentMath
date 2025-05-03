#include "Framework.h"
#include "Engine/Application.h"

#include "Engine/Time.h"
#include "Engine/Renderer/Renderer.h"
#include "Utils/Parallel.h"

using namespace Silent::Renderer;
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

        // Initialize input.
        Log("Initializing input...");
        _input.Initialize();

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
        _input.Deinitialize();

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
        // Update input state.
        _input.Update();

        // TODO: Update game state here.

        // Poll events.
        while (SDL_PollEvent(&_event))
        {
            // Handle each event
            switch (_event.type)
            {
                case SDL_EVENT_QUIT:
                {
                    _isRunning = false;
                    break;
                }

                default:
                    break;
            }
        }
    }

    void ApplicationManager::Render()
    {
        if (g_Time.GetTicks() <= 0)
        {
            return;
        }

        // Wait for previous frame to finish rendering.
        static auto prevFrameFuture = std::future<void>();
        if (prevFrameFuture.valid())
        {
            prevFrameFuture.wait();
        }

        // TODO: Parallelism requires storing a separate render buffer for game data
        // so that it doesn't get overwritten by `ApplicationManager::Update` while 
        // `_renderer.Update` runs in the background.

        // Render scene.
        _renderer.Update();
        //prevFrameFuture = g_Parallel.AddTask([this]() { _renderer.Update(); });
    }
}
