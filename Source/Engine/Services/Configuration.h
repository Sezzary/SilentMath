#pragma once

#include "Engine/Input/Bindings.h"

using namespace Silent::Input;

namespace Silent::Services
{
    enum class OsType
    {
        Windows,
        MacOs,
        Linux
    };

    struct Settings
    {
        static constexpr bool DEFAULT_IS_FULLSCREEN     = false;
        static constexpr auto DEFAULT_SCREEN_RESOLUTION = Vector2i(800, 600);
        static constexpr int  DEFAULT_MOUSE_SENSITIVITY = 6;
        // etc.

        // Graphics

        bool     IsFullscreen     = false;
        Vector2i ScreenResolution = Vector2i(800, 600);

        // Input

        int              MouseSensitivity             = 0;
        BindingProfileId ActiveKeyboardMouseProfileId = BindingProfileId::DefaultKeyboardMouse0;
        BindingProfileId ActiveGamepadProfileId       = BindingProfileId::DefaultGamepad0;
        BindingProfile   KeyboardMouseBindings        = DEFAULT_USER_KEYBOARD_MOUSE_BINDING_PROFILE_0;
        BindingProfile   GamepadBindings              = DEFAULT_USER_GAMEPAD_BINDING_PROFILE_0;
    };

    class ConfigurationManager
    {
    public:
        // Constants
        
#if defined(_WIN32) || defined(_WIN64)
        static constexpr auto OS_TYPE = OsType::Windows;
#elif defined(__APPLE__)
        static constexpr auto OS_TYPE = OsType::MacOs;
#elif defined(__linux__)
        static constexpr auto OS_TYPE = OsType::Linux;
#endif

    private:
        static constexpr char APP_FOLDER_NAME[] = "Silent Engine";
        static constexpr char SETTINGS_PATH[]   = "Settings.json";

        // Fields

        std::filesystem::path _appDir   = {};
        Settings              _settings = {};

    public:
        // Constructors

        ConfigurationManager() = default;

        // Getters

        Settings& GetSettings();

        // Utilities

        void Initialize();
        void SaveSettings();
        void LoadSettings();
        void CreateSettingsFile();

    private:
        // Helpers

        void SetDefaultSettings();

        OsType                GetOsType() const;
        std::filesystem::path GetAppDir() const;
    };

    extern ConfigurationManager g_Config;
}
