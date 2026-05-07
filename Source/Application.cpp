#include "Framework.h"
#include "Application.h"

#include "Assets/AssetStreamer.h"
#include "Assets/Fonts.h"
#include "Assets/TranslationKeys.h"
#include "Audio/Audio.h"
#include "Game/Entry.h"
#include "Input/Input.h"
#include "Renderer/Renderer.h"
#include "Savegame/Savegame.h"
#include "Services/Clock.h"
#include "Services/Filesystem.h"
#include "Services/Options.h"
#include "Services/Platform.h"
#include "Services/Toasts.h"
#include "Utils/Font.h"
#include "Utils/Parallel.h"
#include "Utils/Translator.h"

using namespace Silent::Assets;
using namespace Silent::Audio;
using namespace Silent::Game;
using namespace Silent::Input;
using namespace Silent::Renderer;
using namespace Silent::Savegame;
using namespace Silent::Services;
using namespace Silent::Utils;

namespace Silent
{
    ApplicationManager g_App = ApplicationManager();

    AssetStreamer& ApplicationManager::GetAssets()
    {
        return _work.Assets;
    }

    AudioManager& ApplicationManager::GetAudio()
    {
        return _work.Audio;
    }

    ClockManager& ApplicationManager::GetClock()
    {
        return _work.Clock;
    }

    ParallelExecutor& ApplicationManager::GetExecutor()
    {
        return _work.Executor;
    }

    FilesystemManager& ApplicationManager::GetFilesystem()
    {
        return _work.Filesystem;
    }

    FontManager& ApplicationManager::GetFonts()
    {
        return _work.Fonts;
    }

    InputManager& ApplicationManager::GetInput()
    {
        return _work.Input;
    }

    OptionsManager& ApplicationManager::GetOptions()
    {
        return _work.Options;
    }

    RendererBase& ApplicationManager::GetRenderer()
    {
        return *_work.Renderer;
    }

    SavegameManager& ApplicationManager::GetSavegame()
    {
        return _work.Savegame;
    }

    ToastManager& ApplicationManager::GetToaster()
    {
        return _work.Toaster;
    }

    VideoPlayer& ApplicationManager::GetVideo()
    {
        return _work.Video;
    }

    TranslationManager& ApplicationManager::GetTranslator()
    {
        return _work.Translator;
    }

    Vector2i ApplicationManager::GetWindowResolution() const
    {
        auto res = Vector2i::Zero;
        SDL_GetWindowSizeInPixels(_window, &res.x, &res.y);
        return res;
    }

    float ApplicationManager::GetDpiScale() const
    {
        float dpiScale = SDL_GetWindowDisplayScale(_window);
        if (dpiScale == 0.0f)
        {
            Debug::Log(Fmt("Failed to get DPI scale: {}", SDL_GetError()), Debug::LogLevel::Warning);
            return 1.0f;
        }

        return dpiScale;
    }

    void ApplicationManager::Initialize()
    {
        constexpr float WINDOW_ASPECT_RATIO_MIN = 3.0f / 4.0f;
        constexpr float WINDOW_ASPECT_RATIO_MAX = 42.0f / 9.0f;
        constexpr float IMGUI_FONT_POINT_SIZE   = 13.0f;

        _isPaused = false;
        _quit     = false;

        // Filesystem and translator.
        _work.Filesystem.Initialize();
        _work.Translator.Initialize(_work.Filesystem.GetAssetsDirectory() / ASSETS_LOCALES_DIR_NAME);

        // Debug.
        Debug::Initialize();
        Debug::Log(Fmt("Starting {} {}...", APP_NAME, APP_VERSION));
        Debug::Log(Fmt("Platform: {}", GetPlatformName()));
        Debug::Log(Fmt("Repository: {}", GIT_REPO));
        Debug::Log(Fmt("Branch: {}", GIT_BRANCH));
        Debug::Log(Fmt("Commit: {}", GIT_COMMIT));

        // Options.
        _work.Options.Initialize();
        _work.Options.Load();

        // Collect window flags.
        int fullscreenFlag = _work.Options->EnableFullscreen ? SDL_WINDOW_FULLSCREEN : 0;
        int maximizedFlag  = _work.Options->EnableMaximized  ? SDL_WINDOW_MAXIMIZED  : 0;
        int windowFlags    = SDL_WINDOW_RESIZABLE | fullscreenFlag | maximizedFlag;

        // SDL.
        if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO))
        {
            throw std::runtime_error(Fmt("Failed to initialize SDL: {}", SDL_GetError()));
        }

        // Create window.
        _window = SDL_CreateWindow(APP_NAME, _work.Options->WindowedSize.x, _work.Options->WindowedSize.y, windowFlags);
        if (_window == nullptr)
        {
            throw std::runtime_error(Fmt("Failed to create window: {}", SDL_GetError()));
        }
        //SDL_SetWindowMinimumSize(_window, (int)RETRO_SCREEN_SPACE_RES.x, (int)RETRO_SCREEN_SPACE_RES.y);
        //SDL_SetWindowAspectRatio(_window, WINDOW_ASPECT_RATIO_MIN, WINDOW_ASPECT_RATIO_MAX);

        // Assets.
        _work.Assets.Initialize(_work.Filesystem.GetAssetsDirectory() / ASSETS_PSX_DIR_NAME);
        _work.Video.Initialize(_work.Filesystem.GetAssetsDirectory() / ASSETS_VIDEO_DIR_NAME);

        // Fonts.
        for (const auto& fontMetadata : FONTS_METADATA)
        {
            _work.Fonts.LoadFont(fontMetadata, _work.Filesystem.GetAssetsDirectory() / ASSETS_FONTS_DIR_NAME, GLYPH_PRECACHE);
        }

        // Set ImGui DPI scale.
        float dpiScale                         = GetDpiScale();
        ImGui::GetIO().FontGlobalScale         = dpiScale;
        ImGui::GetIO().DisplayFramebufferScale = ImVec2{ dpiScale, dpiScale };

        // Set ImGui font.
        auto fontDir = _work.Filesystem.GetAssetsDirectory() / ASSETS_FONTS_DIR_NAME / "NotoSansMono.ttf";
        try
        {
            ImGui::GetIO().Fonts->AddFontFromFileTTF(fontDir.string().c_str(), IMGUI_FONT_POINT_SIZE * dpiScale);
        }
        catch(const std::exception& ex)
        {
            Debug::Log(Fmt("Failed to set ImGui font to `NotoSansMono.ttf`: {}.", ex.what()), Debug::LogLevel::Warning);
        }

        // Renderer.
        _work.Renderer = CreateRenderer(RendererType::SdlGpu);
        if (_work.Renderer == nullptr)
        {
            throw std::runtime_error("Failed to create renderer.");
        }
        _work.Renderer->Initialize(*_window);

        // Audio.
        _work.Audio.Initialize();

        // Input.
        _work.Input.Initialize();

        // Show fullscreen toggle toaster hint.
        _work.Toaster.Add(_work.Translator((PLATFORM_TYPE == PlatformType::MacOs) ? KEY_SYS_FULLSCREEN_HINT_MAC :
                                                                                    KEY_SYS_FULLSCREEN_HINT_GENERIC));

        Debug::Log("Startup complete.");
    }

    void ApplicationManager::Deinitialize()
    {
        Debug::Log("Shutting down...");

        // Workspace.
        _work.Audio.Deinitialize();
        _work.Input.Deinitialize();
        _work.Renderer->Deinitialize();

        // SDL.
        SDL_DestroyWindow(_window);
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        SDL_Quit();

        // Finish.
        Debug::Log("Shutdown complete.");
    }

    void ApplicationManager::Run()
    {
        _work.Clock.Initialize();

        while (!_quit)
        {
            _work.Clock.Update();
            PollEvents();

            if (!_isPaused)
            {
                Update();
                Render();
            }

            _work.Clock.WaitForNextTick();
        }
    }

    void ApplicationManager::Quit()
    {
        _quit = true;
    }

    void ApplicationManager::ToggleFullscreen()
    {
        if (SDL_SetWindowFullscreen(_window, !_work.Options->EnableFullscreen))
        {
            Debug::Log("Toggled fullscreen mode.", Debug::LogLevel::Info, Debug::LogMode::All, true);
            return;
        }

        Debug::Log(Fmt("Failed to toggle fullscreen mode: {}", SDL_GetError()), Debug::LogLevel::Warning);
    }

    void ApplicationManager::ToggleCursor()
    {
        // Show.
        if ((!_work.Options->EnableFullscreen || Debug::g_Work.EnablePowerMenu) && !SDL_CursorVisible())
        {
            if (!SDL_ShowCursor())
            {
                Debug::Log(Fmt("Failed to show cursor: {}", SDL_GetError()), Debug::LogLevel::Warning);
            }
        }
        // Hide.
        else if (_work.Options->EnableFullscreen && SDL_CursorVisible())
        {
            if (!SDL_HideCursor())
            {
                Debug::Log(Fmt("Failed to hide cursor: {}", SDL_GetError()), Debug::LogLevel::Warning);
            }
        }
    }

    void ApplicationManager::TogglePowerMenu()
    {
        if (!_work.Options->EnablePowerMode)
        {
            return;
        }

        Debug::g_Work.EnablePowerMenu = !Debug::g_Work.EnablePowerMenu;
        Debug::g_Work.Page            = Debug::g_Work.EnablePowerMenu ? Debug::Page::Renderer : Debug::Page::None;
        ToggleCursor();

        Debug::Log("Toggled power menu.", Debug::LogLevel::Info, Debug::LogMode::All, true);
    }

    void ApplicationManager::Update()
    {
        // Update input.
        _work.Input.Update(*_window, _mouseWheelAxis);

        // Tick game state.
        //if (_work.Clock.GetTicks() > 0)
        for (int i = 0; i < _work.Clock.GetTicks(); i++)
        {
            Entry();
        }

        // Update audio.
        _work.Audio.Update();

        // @debug
        /*if (_work.Input.GetAction(In::Up).IsClicked())
        {
            _work.Toaster.Add("Hello I am a toast.");
        }*/

        // Update debug and toasts.
        Debug::Update();
        _work.Toaster.Update();
    }

    void ApplicationManager::Render()
    {
        // Wait for previous frame to finish rendering.
        if (_prevFrameFuture.valid())
        {
            _prevFrameFuture.wait();
        }

        // Prepare renderer for new frame.
        _work.Renderer->PrepareFrameData();
        _work.Renderer->PrepareFrameResources();

        // Render frame asynchronously.
        if (_work.Options->EnableParallelism)
        {
            _prevFrameFuture = std::async(std::launch::async, TASK(_work.Renderer->Update()));
        }
        else
        {
            _work.Renderer->Update();
        }
    }

    void ApplicationManager::PollEvents()
    {
        auto event = SDL_Event{};
        while (SDL_PollEvent(&event))
        {
            if (Debug::g_Work.EnablePowerMenu)
            {
                ImGui_ImplSDL3_ProcessEvent(&event);
            }

            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                {
                    Quit();
                    break;
                }
                case SDL_EVENT_WINDOW_RESIZED:
                {
                    // Ignore if fullscreen or maximized.
                    auto windowFlags = SDL_GetWindowFlags(_window);
                    if (windowFlags & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_MAXIMIZED))
                    {
                        break;
                    }

                    // Get new resolution.
                    auto res = Vector2i::Zero;
                    SDL_GetWindowSizeInPixels(_window, &res.x, &res.y);
                    if (_work.Options->WindowedSize == res)
                    {
                        break;
                    }

                    // Update options.
                    _work.Options->WindowedSize = res;
                    _work.Options.Save();

                    // Update render size.
                    _work.Renderer->SignalResize();
                    break;
                }
                case SDL_EVENT_WINDOW_MAXIMIZED:
                case SDL_EVENT_WINDOW_RESTORED:
                {
                    // Update options.
                    auto windowFlags               = SDL_GetWindowFlags(_window);
                    _work.Options->EnableMaximized = windowFlags & SDL_WINDOW_MAXIMIZED;
                    _work.Options.Save();

                    // Update render size.
                    _work.Renderer->SignalResize();
                    break;
                }
                case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
                case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
                {
                    // Update options.
                    _work.Options->EnableFullscreen = event.type == SDL_EVENT_WINDOW_ENTER_FULLSCREEN;
                    _work.Options.Save();

                    // Show/hide cursor.
                    ToggleCursor();

                    // Update render size.
                    _work.Renderer->SignalResize();
                    break;
                }
                case SDL_EVENT_MOUSE_WHEEL:
                {
                    // Update mouse wheel axis input state. Cannot be done by input subsystem due to SDL limitations.
                    _mouseWheelAxis = Vector2(event.wheel.x, event.wheel.y);
                    break;
                }
                case SDL_EVENT_GAMEPAD_ADDED:
                {
                    // Attempt connecting gamepad.
                    _work.Input.ConnectGamepad(event.cdevice.which);
                    break;
                }
                case SDL_EVENT_GAMEPAD_REMOVED:
                {
                    // Attempt disconnecting gamepad.
                    _work.Input.DisconnectGamepad(event.cdevice.which);
                    break;
                }
                case SDL_EVENT_WINDOW_MOUSE_ENTER:
                {
                    // @todo
                    break;
                }
                case SDL_EVENT_WINDOW_MOUSE_LEAVE:
                {
                    // @todo
                    break;
                }
                case SDL_EVENT_WINDOW_FOCUS_GAINED:
                {
                    // Resume application.
                    _isPaused = false;
                    break;
                }
                case SDL_EVENT_WINDOW_FOCUS_LOST:
                {
                    // Pause application.
                    _isPaused = true;
                    break;
                }
            }
        }
    }
}
