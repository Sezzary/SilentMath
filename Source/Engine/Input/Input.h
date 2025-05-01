#pragma once

namespace Silent::Input
{
    class InputManager
    {
    public:
        // Constructors

        InputManager() = default;

        // Utilities

        void Initialize();
        void Deinitialize();
        void Update();

    };
}
