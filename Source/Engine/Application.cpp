#include "Framework.h"
#include "Engine/Application.h"

#include "Engine/Time.h"

namespace Silent
{
    void ApplicationManager::Initialize()
    {
        // SDL.
        bool sdlStatus = SDL_Init(SDL_INIT_VIDEO);
        Assert(sdlStatus, "Failed to initialize SDL.");

        // Time.
        g_Time.Initialize();

        Log("Initialization complete.");
    }

    void ApplicationManager::Run()
    {
        // Create window.
        _window = SDL_CreateWindow(WINDOW_NAME, 800, 600, SDL_WINDOW_VULKAN);
        Assert(_window != nullptr, "Failed to create window.");

        // Keep window open until closed by user.
        SDL_Event e;
        bool quit = false;
        while (!quit)
        {
            while (SDL_PollEvent(&e) != 0)
            {
                if (e.type == SDL_EVENT_QUIT)
                {
                    quit = true;
                }
            }
        }

        // SDL.
        SDL_DestroyWindow(_window);
        SDL_Quit();

        while (true)
        {
            g_Time.Update();

            // Update game state.
            // Render.

            g_Time.WaitForNextTick();
        }
    }

    void ApplicationManager::Deinitialize()
    {

        Log("Deinitialization complete.");
    }
}
