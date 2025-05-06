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

    struct SettingsData
    {
        // Input

        int            MouseSensitivity      = 6;
        BindingProfile KeyboardMouseBindings = DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE_0;
        BindingProfile GamepadBindings       = DEFAULT_GAMEPAD_BINDING_PROFILE_0;
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
        static constexpr char APP_NAME[]      = "SilentEngine";
        static constexpr char SETTINGS_PATH[] = "Settings.json";

        // Fields

        std::filesystem::path _appDir   = {};
        SettingsData          _settings = {};

    public:
        // Constructors

        ConfigurationManager() = default;

        // Getters

        SettingsData& GetSettings();

        // Utilities

        void Initialize();
        void SaveSettings();
        void LoadSettings();
        void CreateSettingsFile();

    private:
        // Helpers

        OsType                GetOsType() const;
        std::filesystem::path GetAppDir() const;
    };
}
