#pragma once

#include "Engine/Renderer/Renderer.h"

using namespace Silent::Renderer;

namespace Silent
{
    class ApplicationManager
    {
    private:
        // Constants

        static constexpr char WINDOW_NAME[] = "Silent Engine";
    
        // Fields

        bool            _isRunning = false;
        SDL_Window*     _window    = nullptr;
        SDL_Event       _event     = {};
        RendererManager _renderer  = RendererManager();

    public:
        // Constructors

        ApplicationManager() = default;
        
        // Utilities

        void Initialize();
        void Deinitialize();
        void Run();

    private:
        // Helpers

        void Update();
        void Render();
    };
}
