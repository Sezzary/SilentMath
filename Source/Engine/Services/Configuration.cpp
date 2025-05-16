#include "Framework.h"
#include "Engine/Services/Configuration.h"

#include "Engine/Input/Action.h"
#include "Engine/Input/Bindings.h"

using namespace Silent::Input;

namespace Silent::Services
{
    ConfigurationManager g_Config = ConfigurationManager();

    Settings& ConfigurationManager::GetSettings()
    {
        return _settings;
    }

    void ConfigurationManager::Initialize()
    {
        char*  val    = nullptr;
        size_t length = 0;

        // Set application directory.
        switch (OS_TYPE)
        {
            case OsType::Windows:
            {
                // Get `APPDATA` path.
                if (_dupenv_s(&val, &length, "APPDATA") == 0 && val != nullptr)
                {
                    auto path = std::filesystem::path(val);
                    free(val);
                    _appDir = path / APP_FOLDER_NAME; 
                }
                break;
            }

            case OsType::MacOs:
            {
                // Get `HOME` path.
                if (_dupenv_s(&val, &length, "HOME") == 0 && val != nullptr)
                {
                    auto path = std::filesystem::path(val);
                    free(val);
                    _appDir = path / APP_FOLDER_NAME; 
                }
                break;
            }

            case OsType::Linux:
            {
                // Get `HOME` path.
                if (_dupenv_s(&val, &length, "HOME") == 0 && val != nullptr)
                {
                    auto path = std::filesystem::path(val);
                    free(val);
                    _appDir = path / APP_FOLDER_NAME; 
                }
                break;
            }

            default:
            {
                break;
            }
        }
        Assert(!_appDir.empty(), "Failed to initialize `ConfigurationManager`.");

        LoadSettings();
    }

    void ConfigurationManager::SaveSettings()
    {
        constexpr int INDENT_SIZE = 4;

        // Collect user keyboard/mouse action-event bindings.
        auto kbMouseBindsJson = json();
        for (const auto& [actionId, eventIds] : _settings.KeyboardMouseBindings)
        {
            auto events = json::array();
            for (const auto& eventId : eventIds)
            {
                events.push_back(eventId);
            }

            kbMouseBindsJson[std::to_string((int)actionId)] = events;
        }

        // Collect user gamepad action-event bindings.
        auto gamepadBindsJson = json();
        for (const auto& [actionId, eventIds] : _settings.KeyboardMouseBindings)
        {
            auto events = json::array();
            for (const auto& eventId : eventIds)
            {
                events.push_back(eventId);
            }

            gamepadBindsJson[std::to_string((int)actionId)] = events;
        }

        // Create settings JSON.
        auto settingsJson = json
        {
            {
                "Graphics",
                {
                    { "IsFullscreen",      _settings.IsFullscreen },
                    { "ScreenResolutionX", _settings.ScreenResolution.x },
                    { "ScreenResolutionY", _settings.ScreenResolution.y },
                }
            },
            {
                "Input",
                {
                    { "MouseSensitivity",      _settings.MouseSensitivity },
                    { "KeyboardMouseBindings", kbMouseBindsJson },
                    { "GamepadBindings",       gamepadBindsJson }
                }
            }
        };

        // Ensure application directory exists.
        auto path = GetAppDir() / SETTINGS_PATH;
        std::filesystem::create_directories(path.parent_path());

        // Write settings file.
        auto outputFile = std::ofstream(path);
        if (outputFile.is_open())
        {
            outputFile << settingsJson.dump(INDENT_SIZE);
            outputFile.close();
        }
    }

    void ConfigurationManager::LoadSettings()
    {
        auto path = GetAppDir() / SETTINGS_PATH;
        
        // Open settings file.
        auto inputFile = std::ifstream(path);
        if (!inputFile.is_open())
        {
            Log("No settings file found. Creating new file.", LogLevel::Info);

            SetDefaultSettings();
            SaveSettings();
            return;
        }

        // Parse into JSON object.
        auto settingsJson = json();
        inputFile >> settingsJson;

        // Load graphics settings.
        const auto& graphicsJson     = settingsJson["Graphics"];
        _settings.IsFullscreen       = graphicsJson.value("IsFullscreen",      false);
        _settings.ScreenResolution.x = graphicsJson.value("ScreenResolutionX", 800);
        _settings.ScreenResolution.y = graphicsJson.value("ScreenResolutionY", 600);

        // Load input settings.
        const auto& input = settingsJson["Input"];
        _settings.MouseSensitivity = input.value("MouseSensitivity", 6);

        const auto& kbMouseBindsJson = input["KeyboardMouseBindings"];
        const auto& gamepadBindsJson = input["GamepadBindings"];

        // Load user action-event bindings.
        for (auto actionGroupId : USER_ACTION_GROUP_IDS)
        {
            const auto& actionIds = ACTION_ID_GROUPS.at(actionGroupId);
            for (auto actionId : actionIds)
            {
                auto actionStr = std::to_string((int)actionId);

                // Keyboard/mouse.
                if (kbMouseBindsJson.contains(actionStr))
                {
                    const auto& eventJson = kbMouseBindsJson[actionStr];
                    auto        events    = std::vector<EventId>{};
                    for (const auto& event : eventJson)
                    {
                        events.push_back((EventId)(event.get<int>()));
                    }

                    _settings.KeyboardMouseBindings[actionId] = std::move(events);
                }
                else
                {
                    try
                    {
                        _settings.KeyboardMouseBindings[actionId] = DEFAULT_USER_KEYBOARD_MOUSE_BINDING_PROFILE_0.at(actionId);
                    }
                    catch (const std::out_of_range& error)
                    {
                        Log("Attempted to load keyboard/mouse action-event binding for invalid action " + std::to_string((int)actionId) + ".", LogLevel::Warning);
                    }
                }

                // Gamepad.
                if (gamepadBindsJson.contains(actionStr))
                {
                    const auto& eventJson = gamepadBindsJson[actionStr];
                    auto        events    = std::vector<EventId>{};
                    for (const auto& event : eventJson)
                    {
                        events.push_back((EventId)(event.get<int>()));
                    }

                    _settings.KeyboardMouseBindings[actionId] = std::move(events);
                }
                else
                {
                    try
                    {
                        _settings.KeyboardMouseBindings[actionId] = DEFAULT_USER_GAMEPAD_BINDING_PROFILE_0.at(actionId);
                    }
                    catch (const std::out_of_range& error)
                    {
                        Log("Attempted to load gamepad action-event binding for invalid action " + std::to_string((int)actionId) + ".", LogLevel::Warning);
                    }
                }
            }
        }
    }

    void ConfigurationManager::SetDefaultSettings()
    {
        // Set graphics settings.
        _settings.IsFullscreen     = Settings::DEFAULT_IS_FULLSCREEN;
        _settings.ScreenResolution = Settings::DEFAULT_SCREEN_RESOLUTION;

        // Set input settings.
        _settings.MouseSensitivity      = Settings::DEFAULT_MOUSE_SENSITIVITY;
        _settings.KeyboardMouseBindings = DEFAULT_USER_KEYBOARD_MOUSE_BINDING_PROFILE_0;
        _settings.GamepadBindings       = DEFAULT_USER_GAMEPAD_BINDING_PROFILE_0;
    }
    
    std::filesystem::path ConfigurationManager::GetAppDir() const
    {
        return _appDir;
    }
}
