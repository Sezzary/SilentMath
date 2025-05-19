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
    class ApplicationManager
    {
    private:
        // Constants

        static constexpr char WINDOW_NAME[] = "Silent Engine";

        // Fields

        bool        _isRunning      = false;
        bool        _isDebugMode    = false;
        bool        _enableDebugGui = false;
        SDL_Window* _window         = nullptr;

        std::unique_ptr<RendererBase> _renderer = nullptr;
        ConfigurationManager          _config   = ConfigurationManager();
        SavegameManager               _savegame = SavegameManager();
        InputManager                  _input    = InputManager();

        Vector2 _mouseWheelAxis = Vector2::Zero;
        
        DebugPage _debugPage = DebugPage::None;

    public:
        // Constructors

        ApplicationManager() = default;

        // Getters

        RendererBase&         GetRenderer();
        ConfigurationManager& GetConfig();
        SavegameManager&      GetSavegame();
        InputManager&         GetInput();
        DebugPage             GetDebugPage();

        // Inquirers

        bool IsDebugMode() const;

        // Utilities

        void Initialize();
        void Deinitialize();
        void Run();

        void ToggleFullscreen();
        void ToggleDebugGui();
        void HandleDebugMenu();

    private:
        // Helpers

        void Update();
        void Render();
        void PollEvents();
    };

    extern ApplicationManager g_App;
}
