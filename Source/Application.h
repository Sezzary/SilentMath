#pragma once

#include "Assets/Assets.h"
#include "Audio/Audio.h"
#include "Input/Input.h"
#include "Renderer/Renderer.h"
#include "Savegame/Savegame.h"
#include "Services/Clock.h"
#include "Services/Filesystem.h"
#include "Services/Options.h"
#include "Services/Toasts.h"
#include "Utils/Font.h"
#include "Utils/Parallel.h"
#include "Utils/Translator.h"

namespace Silent
{
    using namespace Assets;
    using namespace Audio;
    using namespace Input;
    using namespace Renderer;
    using namespace Savegame;
    using namespace Services;
    using namespace Utils;

    constexpr char APP_NAME[]    = "Silent Engine";
    constexpr char APP_VERSION[] = "0.1.0";

    /** @brief Application workspace. Keeps all subsystems. */
    struct ApplicationWork
    {
        AssetManager                  Assets     = AssetManager();
        AudioManager                  Audio      = AudioManager();
        FilesystemManager             Filesystem = FilesystemManager();
        FontManager                   Fonts      = FontManager();
        InputManager                  Input      = InputManager();
        OptionsManager                Options    = OptionsManager();
        ParallelExecutor              Executor   = ParallelExecutor();
        std::unique_ptr<RendererBase> Renderer   = nullptr;
        SavegameManager               Savegame   = SavegameManager();
        ClockManager                  Clock      = ClockManager();
        TranslationManager            Translator = TranslationManager();
        ToastManager                  Toaster    = ToastManager();
    };

    class ApplicationManager
    {
    private:
        // =======
        // Fields
        // =======

        SDL_Window*       _window          = nullptr;
        std::future<void> _prevFrameFuture = std::future<void>();
        bool              _isPaused        = false;
        bool              _quit            = false;

        ApplicationWork _work           = {};
        Vector2         _mouseWheelAxis = Vector2::Zero;

    public:
        // =============
        // Constructors
        // =============

        /** Constructs a default uninitialized `ApplicationManager`. */
        ApplicationManager() = default;

        // ========
        // Getters
        // ========

        /** @brief Gets the assets subsystem instance.
         *
         * @return Assets instance.
         */
        AssetManager& GetAssets();

        /** @brief Gets the audio subsystem instance.
         *
         * @return Audio instance.
         */
        AudioManager& GetAudio();

        /** @brief Gets the executor subsystem instance.
         *
         * @return Executor instance.
         */
        ParallelExecutor& GetExecutor();

        /** @brief Gets the filesystem subsystem instance.
         *
         * @return Filesystem instance.
         */
        FilesystemManager& GetFilesystem();

        /** @brief Gets the fonts subsystem instance.
         *
         * @return Fonts instance.
         */
        FontManager& GetFonts();

        /** @brief Gets the input subsystem instance.
         *
         * @return Input instance.
         */
        InputManager& GetInput();

        /** @brief Gets the options subsystem instance.
         *
         * @return Options instance.
         */
        OptionsManager& GetOptions();

        /** @brief Gets the renderer subsystem instance.
         *
         * @return Renderer instance.
         */
        RendererBase& GetRenderer();

        /** @brief Gets the savegame subsystem instance.
         *
         * @return Savegame instance.
         */
        SavegameManager& GetSavegame();

        /** @brief Gets the clock subsystem instance.
         *
         * @return Clock instance.
         */
        ClockManager& GetClock();

        /** @brief Gets the translator subsystem instance.
         *
         * @return Translator instance.
         */
        TranslationManager& GetTranslator();

        /** @brief Gets the toaster subsystem instance.
         *
         * @return Toaster instance.
         */
        ToastManager& GetToaster();

        /** @brief Gets the window resolution.
         *
         * @return Window resolution
         */
        Vector2i GetWindowResolution() const;

        // ==========
        // Utilities
        // ==========

        /** @brief Initializes the application and its subsystems. */
        void Initialize();

        /** @brief Gracefully deinitializes the application and its subsystems. */
        void Deinitialize();

        /** @brief Runs the application loop at at a fixed timestep. */
        void Run();

        /** @brief Prompts the application to run the shutdown procedure. */
        void Quit();

        /** @brief Toggles between fullscreen and windowed modes. */
        void ToggleFullscreen();

        /** @brief Toggles the mouse cursor on and off. */
        void ToggleCursor();

        /** @brief Toggles the power menu on and off. */
        void TogglePowerMenu();

    private:
        // ========
        // Helpers
        // ========

        /** @brief Updates the application state. */
        void Update();

        /** @brief Renders the application scene. */
        void Render();

        /** @brief Polls window events to handle quitting, window resizing, toggling fullscreen mode, and connecting or disconnecting a gamepad.
         *
         * @note Additionally polls mouse wheel input as a workaround to SDL's input device query limitations.
         */
        void PollEvents();
    };

    extern ApplicationManager g_App;
}
