#pragma once

#include "Engine/Input/Input.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Services/Configuration.h"
#include "Engine/Services/Savegame.h"

using namespace Silent::Input;
using namespace Silent::Renderer;
using namespace Silent::Services;

namespace Silent
{
    struct ApplicationWork
    {
        // TODO: `g_SysWork`, `g_GameWork`, and probably other globals would go here.

        std::unique_ptr<RendererBase> Renderer = nullptr;
        InputManager                  Input    = InputManager();
        SavegameManager               Savegame = SavegameManager();
        ConfigurationManager          Config   = ConfigurationManager();
    };

    class ApplicationManager
    {
    private:
        // Constants

        static constexpr char WINDOW_NAME[] = "Silent Engine";

        // Fields

        SDL_Window* _window         = nullptr;
        Vector2     _mouseWheelAxis = Vector2::Zero;

        ApplicationWork _work           = {};
        bool            _isRunning      = false;
        bool            _enableDebugGui = true; // TODO: Temporarily hard-set to `true`.
        DebugPage       _debugPage      = DebugPage::None;

    public:
        // Constructors

        ApplicationManager() = default;

        // Getters

        RendererBase&         GetRenderer();
        InputManager&         GetInput();
        SavegameManager&      GetSavegame();
        ConfigurationManager& GetConfig();
        DebugPage             GetDebugPage();

        // Setters

        void SetDebugPage(DebugPage page);

        // Utilities

        void Initialize();
        void Deinitialize();
        void Run();

        void ToggleFullscreen();
        void ToggleDebugGui();
        void HandleDebugGui();

    private:
        // Helpers

        void Update();
        void Render();
        void PollEvents();
    };

    extern ApplicationManager g_App;
}
