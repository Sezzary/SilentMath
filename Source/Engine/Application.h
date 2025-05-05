#pragma once

#include "Engine/Configuration.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Input/Input.h"

using namespace Silent::Input;
using namespace Silent::Renderer;

namespace Silent
{
    class ApplicationManager
    {
    private:
        // Constants

        static constexpr char WINDOW_NAME[] = "Silent Engine";
    
        // Fields

        bool        _isRunning = false;
        SDL_Window* _window    = nullptr;
        SDL_Event   _event     = {};

        ConfigurationManager _config    = ConfigurationManager();
        InputManager         _input     = InputManager();
        RendererManager      _renderer  = RendererManager();

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
