#include "Framework.h"
#include "Engine/Application.h"

#include "Engine/Time.h"
#include "Engine/Renderer/Renderer.h"

using namespace Silent::Renderer;

namespace Silent
{
    void ApplicationManager::Initialize()
    {
        // Initialize SDL.
        Log("Initializing SDL...");
        bool sdlStatus = SDL_Init(SDL_INIT_VIDEO);
        Assert(sdlStatus, "Failed to initialize SDL.");

        // Create window.
        _window = SDL_CreateWindow(WINDOW_NAME, 800, 600, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
        Assert(_window != nullptr, "Failed to create window.");

        // Vulkan test.
        uint extCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
        Log("Extension supported: " + std::to_string(extCount));

        _isRunning = true;
        Log("Initialization complete.");
    }

    void ApplicationManager::Deinitialize()
    {
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
        SDL_PollEvent(&_event);
        _isRunning = _event.type != SDL_EVENT_QUIT;
    }

    void ApplicationManager::Render()
    {
        g_Renderer.Update();
    }
}
