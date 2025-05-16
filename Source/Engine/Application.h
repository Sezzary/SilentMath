#pragma once

#include "Engine/Input/Input.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Services/Configuration.h"

using namespace Silent::Input;
using namespace Silent::Renderer;
using namespace Silent::Services;

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

        ConfigurationManager _config = ConfigurationManager();
        InputManager         _input  = InputManager();

        Vector2 _mouseWheelAxis = Vector2::Zero;

    public:
        // Constructors

        ApplicationManager() = default;

        ConfigurationManager& GetConfig();
        InputManager&         GetInput();

        // Utilities

        void Initialize();
        void Deinitialize();
        void Run();

    private:
        // Helpers

        void Update();
        void Render();
        void PollEvents();
    };
}
