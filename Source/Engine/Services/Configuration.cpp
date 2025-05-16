#include "Framework.h"
#include "Engine/Services/Configuration.h"

#include "Engine/Input/Action.h"
#include "Engine/Input/Bindings.h"

using namespace Silent::Input;

namespace Silent::Services
{
    ConfigurationManager g_Config = ConfigurationManager();

    std::filesystem::path ConfigurationManager::GetAppDir() const
    {
        return _appDir;
    }

    Options& ConfigurationManager::GetOptions()
    {
        return _options;
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

        LoadOptions();
    }

    void ConfigurationManager::SaveOptions()
    {
        constexpr int INDENT_SIZE = 4;

        // Collect user keyboard/mouse action-event bindings.
        auto kbMouseBindsJson = json();
        for (const auto& [actionId, eventIds] : _options.KeyboardMouseBindings)
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
        for (const auto& [actionId, eventIds] : _options.KeyboardMouseBindings)
        {
            auto events = json::array();
            for (const auto& eventId : eventIds)
            {
                events.push_back(eventId);
            }

            gamepadBindsJson[std::to_string((int)actionId)] = events;
        }

        // Create options JSON.
        auto optionsJson = json
        {
            {
                "Graphics",
                {
                    { "IsFullscreen",      _options.IsFullscreen },
                    { "ScreenResolutionX", _options.ScreenResolution.x },
                    { "ScreenResolutionY", _options.ScreenResolution.y }
                }
            },
            {
                "Gameplay",
                {
                    { "BrightnessLevel",   _options.BrightnessLevel },
                    { "EnableVibration",   _options.EnableVibration },
                    { "EnableAutoLoad",    _options.EnableAutoLoad },
                    { "SoundType",         _options.SoundType },
                    { "BgmVolume",         _options.BgmVolume },
                    { "SeVolume",          _options.SeVolume },
                    { "WeaponControl",     _options.WeaponControl },
                    { "BloodColor",        _options.BloodColor },
                    { "ViewControl",       _options.ViewControl },
                    { "RetreatTurn",       _options.RetreatTurn },
                    { "WalkRunControl",    _options.WalkRunControl },
                    { "DisableAutoAiming", _options.DisableAutoAiming },
                    { "ViewMode",          _options.ViewMode },
                    { "BulletAdjust",      _options.BulletAdjust }
                }
            },
            {
                "Input",
                {
                    { "KeyboardMouseBindings",        kbMouseBindsJson },
                    { "GamepadBindings",              gamepadBindsJson },
                    { "ActiveKeyboardMouseProfileId", _options.ActiveKeyboardMouseProfileId },
                    { "ActiveGamepadProfileId",       _options.ActiveGamepadProfileId },
                    { "MouseSensitivity",             _options.MouseSensitivity }
                }
            }
        };

        // Ensure application directory exists.
        auto path = GetAppDir() / OPTIONS_PATH;
        std::filesystem::create_directories(path.parent_path());

        // Write options file.
        auto outputFile = std::ofstream(path);
        if (outputFile.is_open())
        {
            outputFile << optionsJson.dump(INDENT_SIZE);
            outputFile.close();
        }
    }

    void ConfigurationManager::LoadOptions()
    {
        auto path = GetAppDir() / OPTIONS_PATH;
        
        // Open options file.
        auto inputFile = std::ifstream(path);
        if (!inputFile.is_open())
        {
            Log("No options file found. Creating new file.", LogLevel::Info);

            SetDefaultOptions();
            SaveOptions();
            return;
        }

        // Parse file into JSON object.
        auto optionsJson = json();
        inputFile >> optionsJson;

        // Load graphics options.
        const auto& graphicsJson    = optionsJson["Graphics"];
        _options.IsFullscreen       = graphicsJson.value("IsFullscreen",      Options::DEFAULT_IS_FULLSCREEN);
        _options.ScreenResolution.x = graphicsJson.value("ScreenResolutionX", Options::DEFAULT_SCREEN_RESOLUTION.x);
        _options.ScreenResolution.y = graphicsJson.value("ScreenResolutionY", Options::DEFAULT_SCREEN_RESOLUTION.y);

        // Load gameplay options.
        const auto& gameplayJson   = optionsJson["Gameplay"];
        _options.BrightnessLevel   = gameplayJson.value("BrightnessLevel",   Options::DEFAULT_BRIGHTNESS_LEVEL);
        _options.EnableVibration   = gameplayJson.value("EnableVibration",   Options::DEFAULT_ENABLE_VIBRATION);
        _options.EnableAutoLoad    = gameplayJson.value("EnableAutoLoad",    Options::DEFAULT_ENABLE_AUTO_LOAD);
        _options.SoundType         = gameplayJson.value("SoundType",         Options::DEFAULT_SOUND_TYPE);
        _options.BgmVolume         = gameplayJson.value("BgmVolume",         Options::DEFAULT_BGM_VOLUME);
        _options.SeVolume          = gameplayJson.value("SeVolume",          Options::DEFAULT_SE_VOLUME);
        _options.WeaponControl     = gameplayJson.value("WeaponControl",     Options::DEFAULT_WEAPON_CONTROL);
        _options.BloodColor        = gameplayJson.value("BloodColor",        Options::DEFAULT_BLOOD_COLOR);
        _options.ViewControl       = gameplayJson.value("ViewControl",       Options::DEFAULT_VIEW_CONTROL);
        _options.RetreatTurn       = gameplayJson.value("RetreatTurn",       Options::DEFAULT_RETREAT_TURN);
        _options.WalkRunControl    = gameplayJson.value("WalkRunControl",    Options::DEFAULT_WALK_RUN_CONTROL);
        _options.DisableAutoAiming = gameplayJson.value("DisableAutoAiming", Options::DEFAULT_DISABLE_AUTO_AIMING);
        _options.ViewMode          = gameplayJson.value("ViewMode",          Options::DEFAULT_VIEW_MODE);
        _options.BulletAdjust      = gameplayJson.value("BulletAdjust",      Options::DEFAULT_BULLET_ADJUST);

        // Load input options.
        const auto& inputJson                 = optionsJson["Input"];
        _options.ActiveKeyboardMouseProfileId = inputJson.value("ActiveKeyboardMouseProfileId", Options::DEFAULT_ACTIVE_KEYBOARD_MOUSE_BINDING_PROFILE_ID);
        _options.ActiveGamepadProfileId       = inputJson.value("ActiveGamepadProfileId",       Options::DEFAULT_ACTIVE_GAMEPAD_BINDING_PROFILE_ID);
        _options.MouseSensitivity             = inputJson.value("MouseSensitivity",             Options::DEFAULT_MOUSE_SENSITIVITY);

        // Load user action-event bindings.
        const auto& kbMouseBindsJson = inputJson["KeyboardMouseBindings"];
        const auto& gamepadBindsJson = inputJson["GamepadBindings"];
        for (auto actionGroupId : USER_ACTION_GROUP_IDS)
        {
            const auto& actionIds = ACTION_ID_GROUPS.at(actionGroupId);
            for (auto actionId : actionIds)
            {
                auto actionStr = std::to_string((int)actionId);

                // Keyboard/mouse.
                if (kbMouseBindsJson.contains(actionStr))
                {
                    const auto& eventsJson = kbMouseBindsJson[actionStr];
                    auto        events     = std::vector<EventId>{};
                    for (const auto& eventJson : eventsJson)
                    {
                        events.push_back((EventId)(eventJson.get<int>()));
                    }

                    _options.KeyboardMouseBindings[actionId] = std::move(events);
                }
                else
                {
                    _options.KeyboardMouseBindings[actionId] = DEFAULT_USER_KEYBOARD_MOUSE_BINDING_PROFILE_0.at(actionId);
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

                    _options.KeyboardMouseBindings[actionId] = std::move(events);
                }
                else
                {
                    _options.KeyboardMouseBindings[actionId] = DEFAULT_USER_GAMEPAD_BINDING_PROFILE_0.at(actionId);
                }
            }
        }
    }

    void ConfigurationManager::SetDefaultOptions()
    {
        // Set graphics options.
        _options.IsFullscreen     = Options::DEFAULT_IS_FULLSCREEN;
        _options.ScreenResolution = Options::DEFAULT_SCREEN_RESOLUTION;

        // Set gameplay options.
        _options.BrightnessLevel   = Options::DEFAULT_BRIGHTNESS_LEVEL;
        _options.EnableVibration   = Options::DEFAULT_ENABLE_VIBRATION;
        _options.EnableAutoLoad    = Options::DEFAULT_ENABLE_AUTO_LOAD;
        _options.SoundType         = Options::DEFAULT_SOUND_TYPE;
        _options.BgmVolume         = Options::DEFAULT_BGM_VOLUME;
        _options.SeVolume          = Options::DEFAULT_SE_VOLUME;
        _options.WeaponControl     = Options::DEFAULT_WEAPON_CONTROL;
        _options.BloodColor        = Options::DEFAULT_BLOOD_COLOR;
        _options.ViewControl       = Options::DEFAULT_VIEW_CONTROL;
        _options.RetreatTurn       = Options::DEFAULT_RETREAT_TURN;
        _options.WalkRunControl    = Options::DEFAULT_WALK_RUN_CONTROL;
        _options.DisableAutoAiming = Options::DEFAULT_DISABLE_AUTO_AIMING;
        _options.ViewMode          = Options::DEFAULT_VIEW_MODE;
        _options.BulletAdjust      = Options::DEFAULT_BULLET_ADJUST;

        // Set input options.
        _options.MouseSensitivity      = Options::DEFAULT_MOUSE_SENSITIVITY;
        _options.KeyboardMouseBindings = DEFAULT_USER_KEYBOARD_MOUSE_BINDING_PROFILE_0;
        _options.GamepadBindings       = DEFAULT_USER_GAMEPAD_BINDING_PROFILE_0;
    }
}
