#include "Framework.h"
#include "Engine/Configuration.h"

#include "Engine/Input/Bindings.h"

using namespace Silent::Input;

namespace Silent
{
    std::filesystem::path ConfigurationManager::GetAppDir() const
    {
        return _appDir;
    }
    
    SettingsData& ConfigurationManager::GetSettings()
    {
        return _settings;
    }

    void ConfigurationManager::Initialize()
    {
        char*  value  = nullptr;
        size_t length = 0;

        // Set application directory.
        switch (OS_TYPE)
        {
            case OsType::Windows:
            {
                // Get `APPDATA` path.
                if (_dupenv_s(&value, &length, "APPDATA") == 0 && value != nullptr)
                {
                    auto path = std::filesystem::path(value);
                    free(value);
                    _appDir = path / APP_NAME; 
                }
                break;
            }

            case OsType::MacOs:
            {
                // Get `HOME` path.
                if (_dupenv_s(&value, &length, "HOME") == 0 && value != nullptr)
                {
                    auto path = std::filesystem::path(value);
                    free(value);
                    _appDir = path / APP_NAME; 
                }
                break;
            }

            case OsType::Linux:
            {
                // Get `HOME` path.
                if (_dupenv_s(&value, &length, "HOME") == 0 && value != nullptr)
                {
                    auto path = std::filesystem::path(value);
                    free(value);
                    _appDir = path / APP_NAME; 
                }
                break;
            }

            default:
                break;
        }
        Assert(!_appDir.empty(), "Failed to initialize `ConfigurationManager`.");

        LoadSettings();
    }

    void ConfigurationManager::SaveSettings()
    {
        auto path = GetAppDir() / SETTINGS_PATH;

        // Open settings file.
        auto inputFile = std::ifstream(path);
        if (!inputFile.is_open())
        {
            // TODO: Create default.
            CreateSettingsFile();
            return;
        }

        // Parse into JSON object.
        auto jsonFile = json();
        inputFile >> jsonFile;

        // TODO: Save settings.
    }

    void ConfigurationManager::LoadSettings()
    {
        auto path = GetAppDir() / SETTINGS_PATH;
        
        // Open settings file.
        auto inputFile = std::ifstream(path);
        if (!inputFile.is_open())
        {
            Log("Failed to load settings. Loading defaults.", LogLevel::Info);
            CreateSettingsFile();
            return;
        }

        // Parse into JSON object.
        auto jsonFile = json();
        inputFile >> jsonFile;
        
        // TODO: Load data into `_settings`.
    }

    void ConfigurationManager::CreateSettingsFile()
    {

    }
}
