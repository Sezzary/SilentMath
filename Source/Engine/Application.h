#pragma once

struct SDL_Window;

namespace Silent
{
    class ApplicationManager
    {
    private:
        // Constants

        static constexpr char WINDOW_NAME[] = "Silent Engine";
    
        // Fields

        SDL_Window* _window = nullptr;

    public:
        // Constructors

        ApplicationManager() = default;
        
        // Utilities

        void Initialize();
        void Run();
        void Deinitialize();
    };
}
