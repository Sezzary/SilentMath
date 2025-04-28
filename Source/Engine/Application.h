#pragma once

namespace Silent
{
    class ApplicationManager
    {
    private:


    public:
        // Constructors

        ApplicationManager() = default;
        
        // Utilities

        void Initialize();
        void Run();
        void Deinitialize();
    };
}
