#pragma once

namespace Silent::Input
{
    struct RawInput
    {
        std::vector<bool> KeyStates     = {};
        Vector2           MousePosition = {};
    };

    class InputManager
    {
    private:
        // Fields

        RawInput _rawInput = {};

    public:
        // Constructors

        InputManager() = default;

        // Utilities

        void Initialize();
        void Deinitialize();
        void Update();

    private:
        // Helpers

        void ReadKeyboard(int& keyStateIdx);
        void ReadMouse(int& keyStateIdx);
        void ReadController(int& keyStateIdx);
    };
}
