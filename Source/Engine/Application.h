#pragma once

#include "Engine/Assets/Assets.h"
#include "Engine/Input/Input.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Services/Configuration.h"
#include "Engine/Services/Savegame/Savegame.h"

namespace Silent
{
    using namespace Assets;
    using namespace Input;
    using namespace Renderer;
    using namespace Services;

    struct ApplicationWork
    {
        // TODO: `g_SysWork`, `g_GameWork`, and probably other globals would go here.

        InputManager                  Input    = InputManager();
        AssetManager                  Assets   = AssetManager();
        SavegameManager               Savegame = SavegameManager();
        ConfigurationManager          Config   = ConfigurationManager();
        std::unique_ptr<RendererBase> Renderer = nullptr;
    };

    class ApplicationManager
    {
    private:
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

        InputManager&         GetInput();
        AssetManager&         GetAssets();
        SavegameManager&      GetSavegame();
        ConfigurationManager& GetConfig();
        RendererBase&         GetRenderer();
        DebugPage             GetDebugPage();

        // Setters

        void SetDebugPage(DebugPage page);

        // Utilities

        void Initialize();
        void Deinitialize();
        void Run();

        void ToggleFullscreen();
        void ToggleDebugGui();

    private:
        // Helpers

        void Update();
        void Render();
        void PollEvents();
    };

    extern ApplicationManager g_App;
}
