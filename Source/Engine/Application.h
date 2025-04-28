#pragma once

namespace Silent
{
    class ApplicationManager
    {
    private:
        // Constants

        static constexpr char WINDOW_NAME[] = "Silent Engine";
    
        // Fields

        SDL_Window* _window    = nullptr;
        SDL_Event   _event     = {};
        bool        _isRunning = false;

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
