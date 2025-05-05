#pragma once

#include "Engine/Input/Bindings.h"

using namespace Silent::Input;

namespace Silent
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

        int            MouseSensitivity = 0;
        //BindingProfile Bindings         = {};
    };

    class ConfigurationManager
    {
    public:
        // Constants
        
#if defined(_WIN32) || defined(_WIN64)
        static constexpr auto OS_TYPE = OsType::Windows;
#elif defined(__APPLE__)
        static constexpr auto OS_TYPE = OsType::Mac;
#elif defined(__linux__)
        static constexpr auto OS_TYPE = OsType::Linux;
#endif

    private:
        static constexpr char APP_FOLDER_NAME[]    = "SilentEngine";
        static constexpr char SETTINGS_FILE_NAME[] = "Settings.json";

        // Fields

        SettingsData _settings = {};

    public:
        // Constructors

        ConfigurationManager() = default;

        // Getters

        SettingsData& GetSettings();

        // Utilities

        void Save();
        void Load();

    private:
        // Helpers

        OsType                GetOsType() const;
        std::filesystem::path GetConfigPath() const;
    };
}
