#include "Framework.h"
#include "Engine/Configuration.h"

#include "Engine/Input/Bindings.h"

using namespace Silent::Input;

namespace Silent
{
    std::filesystem::path ConfigurationManager::GetConfigPath() const
    {
        switch (SETTINGS_FILE_NAME)
        {
            case OsType::Windows:
                return std::filesystem::path(getenv("APPDATA")) / "SilentEngine" / SETTINGS_FILE_NAME;

            case OsType::MacOs:
                return std::filesystem::path(getenv("HOME")) / "Library" / "Application Support" / "SilentEngine" / SETTINGS_FILE_NAME;

            case OsType::Linux:
                return std::filesystem::path(getenv("HOME")) / ".config" / "SilentEngine" / SETTINGS_FILE_NAME;

            default:
                break;
        }

        return {};
    }
    
    SettingsData& ConfigurationManager::GetSettings()
    {
        return _settings;
    }

    void ConfigurationManager::Save()
    {
        auto path = GetConfigPath();
        // TODO
    }

    void ConfigurationManager::Load()
    {
        auto path = GetConfigPath();
        // TODO
    }
}
