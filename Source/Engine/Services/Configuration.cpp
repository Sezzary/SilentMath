#include "Framework.h"
#include "Engine/Services/Configuration.h"

#include "Engine/Input/Action.h"
#include "Engine/Input/Binding.h"

using namespace Silent::Input;

namespace Silent::Services
{
    constexpr char KEY_GRAPHICS[]                                 = "Graphics";
    constexpr char KEY_INPUT[]                                    = "Input";
    constexpr char KEY_GAMEPLAY[]                                 = "Gameplay";
    constexpr char KEY_SYSTEM[]                                   = "System";
    constexpr char KEY_WINDOWED_SIZE_X[]                          = "WindowedSizeX";
    constexpr char KEY_WINDOWED_SIZE_Y[]                          = "WindowedSizeY";
    constexpr char KEY_ENABLE_FULLSCREEN[]                        = "EnableFullscreen";
    constexpr char KEY_ENABLE_MAXIMIZED[]                         = "EnableMaximized";
    constexpr char KEY_BRIGHTNESS_LEVEL[]                         = "BrightnessLevel";
    constexpr char KEY_TEXTURE_FILTER_TYPE[]                      = "TextureFilterType";
    constexpr char KEY_ENABLE_DITHERING[]                         = "EnableDithering";
    constexpr char KEY_ENABLE_CRT_FILTER[]                        = "EnableCrtFilter";
    constexpr char KEY_ENABLE_VERTEX_JITTER[]                     = "EnableVertexJitter";
    constexpr char KEY_RENDER_SCALE_TYPE[]                        = "RenderScaleType";
    constexpr char KEY_ASPECT_RATIO_TYPE[]                        = "AspectRatioType";
    constexpr char KEY_ENABLE_AUTO_LOAD[]                         = "EnableAutoLoad";
    constexpr char KEY_ENABLE_SUBTITLES[]                         = "EnableSubtitles";
    constexpr char KEY_SOUND_TYPE[]                               = "SoundType";
    constexpr char KEY_BGM_VOLUME[]                               = "BgmVolume";
    constexpr char KEY_SE_VOLUME[]                                = "SeVolume";
    constexpr char KEY_BLOOD_COLOR[]                              = "BloodColor";
    constexpr char KEY_BULLET_ADJUST[]                            = "BulletAdjust";
    constexpr char KEY_KEYBOARD_MOUSE_BINDINGS[]                  = "KeyboardMouseBindings";
    constexpr char KEY_ACTIVE_KEYBOARD_MOUSE_BINDING_PROFILE_ID[] = "ActiveKeyboardMouseBindingProfileId";
    constexpr char KEY_GAMEPAD_BINDINGS[]                         = "GamepadBindings";
    constexpr char KEY_ACTIVE_GAMEPAD_BINDING_PROFILE_ID[]        = "ActiveGamepadBindingProfileId";
    constexpr char KEY_ENABLE_VIBRATION[]                         = "EnableVibration";
    constexpr char KEY_MOUSE_SENSITIVITY[]                        = "MouseSensitivity";
    constexpr char KEY_WEAPON_CONTROL[]                           = "WeaponControl";
    constexpr char KEY_VIEW_CONTROL[]                             = "ViewControl";
    constexpr char KEY_RETREAT_TURN[]                             = "RetreatTurn";
    constexpr char KEY_WALK_RUN_CONTROL[]                         = "WalkRunControl";
    constexpr char KEY_DISABLE_AUTO_AIMING[]                      = "DisableAutoAiming";
    constexpr char KEY_VIEW_MODE[]                                = "ViewMode";
    constexpr char KEY_ENABLE_TOASTS[]                            = "EnableToasts";

    constexpr auto DEFAULT_WINDOWED_SIZE                            = Vector2i(800, 600);
    constexpr bool DEFAULT_ENABLE_MAXIMIZED                         = false;
    constexpr bool DEFAULT_ENABLE_FULLSCREEN                        = false;
    constexpr int  DEFAULT_BRIGHTNESS_LEVEL                         = 3;
    constexpr auto DEFAULT_RENDER_SCALE_TYPE                        = RenderScaleType::Native;
    constexpr auto DEFAULT_ASPECT_RATIO_TYPE                        = AspectRatioType::Native;
    constexpr auto DEFAULT_TEXTURE_FILTER_TYPE                      = TextureFilteringType::Nearest;
    constexpr bool DEFAULT_ENABLE_DITHERING                         = true;
    constexpr bool DEFAULT_ENABLE_CRT_FILTER                        = false;
    constexpr bool DEFAULT_ENABLE_VERTEX_JITTER                     = false;
    constexpr bool DEFAULT_ENABLE_AUTO_LOAD                         = false;
    constexpr bool DEFAULT_ENABLE_SUBTITLES                         = true;
    constexpr int  DEFAULT_SOUND_TYPE                               = 0;
    constexpr int  DEFAULT_BGM_VOLUME                               = 16;
    constexpr int  DEFAULT_SE_VOLUME                                = 16;
    constexpr int  DEFAULT_BLOOD_COLOR                              = 0;
    constexpr int  DEFAULT_BULLET_ADJUST                            = 0;
    constexpr auto DEFAULT_ACTIVE_KEYBOARD_MOUSE_BINDING_PROFILE_ID = BindingProfileId::KeyboardMouseType1;
    constexpr auto DEFAULT_ACTIVE_GAMEPAD_BINDING_PROFILE_ID        = BindingProfileId::GamepadType1;
    constexpr bool DEFAULT_ENABLE_VIBRATION                         = true;
    constexpr int  DEFAULT_MOUSE_SENSITIVITY                        = 6;
    constexpr int  DEFAULT_WEAPON_CONTROL                           = 1;
    constexpr int  DEFAULT_VIEW_CONTROL                             = 0;
    constexpr int  DEFAULT_RETREAT_TURN                             = 0;
    constexpr int  DEFAULT_WALK_RUN_CONTROL                         = 0;
    constexpr bool DEFAULT_DISABLE_AUTO_AIMING                      = false;
    constexpr int  DEFAULT_VIEW_MODE                                = 0;
    constexpr bool DEFAULT_ENABLE_TOASTS                            = true;

    std::filesystem::path ConfigurationManager::GetAppDirPath() const
    {
        return _appDirPath;
    }

    Options& ConfigurationManager::GetOptions()
    {
        return _options;
    }

    void ConfigurationManager::SetDefaultGraphicsOptions()
    {
        _options.WindowedSize       = DEFAULT_WINDOWED_SIZE;
        _options.EnableMaximized    = DEFAULT_ENABLE_MAXIMIZED;
        _options.EnableFullscreen   = DEFAULT_ENABLE_FULLSCREEN;
        _options.BrightnessLevel    = DEFAULT_BRIGHTNESS_LEVEL;
        _options.RenderScaleType    = DEFAULT_RENDER_SCALE_TYPE;
        _options.AspectRatioType    = DEFAULT_ASPECT_RATIO_TYPE;
        _options.TextureFilterType  = DEFAULT_TEXTURE_FILTER_TYPE;
        _options.EnableDithering    = DEFAULT_ENABLE_DITHERING;
        _options.EnableCrtFilter    = DEFAULT_ENABLE_CRT_FILTER;
        _options.EnableVertexJitter = DEFAULT_ENABLE_VERTEX_JITTER;
    }

    void ConfigurationManager::SetDefaultGameplayOptions()
    {
        _options.EnableAutoLoad  = DEFAULT_ENABLE_AUTO_LOAD;
        _options.EnableSubtitles = DEFAULT_ENABLE_SUBTITLES;
        _options.SoundType       = DEFAULT_SOUND_TYPE;
        _options.BgmVolume       = DEFAULT_BGM_VOLUME;
        _options.SeVolume        = DEFAULT_SE_VOLUME;
        _options.BloodColor      = DEFAULT_BLOOD_COLOR;
        _options.BulletAdjust    = DEFAULT_BULLET_ADJUST;
    }

    void ConfigurationManager::SetDefaultInputKmBindingsOptions()
    {
        _options.KeyboardMouseBindings = USER_KEYBOARD_MOUSE_BINDING_PROFILE_TYPE_1;
    }

    void ConfigurationManager::SetDefaultInputGamepadCustomBindingOptions()
    {
        _options.GamepadBindings = USER_GAMEPAD_BINDING_PROFILE_TYPE_1;
    }

    void ConfigurationManager::SetDefaultInputControlsOptions()
    {
        _options.EnableVibration   = DEFAULT_ENABLE_VIBRATION;
        _options.MouseSensitivity  = DEFAULT_MOUSE_SENSITIVITY;
        _options.WeaponControl     = DEFAULT_WEAPON_CONTROL;
        _options.ViewControl       = DEFAULT_VIEW_CONTROL;
        _options.RetreatTurn       = DEFAULT_RETREAT_TURN;
        _options.WalkRunControl    = DEFAULT_WALK_RUN_CONTROL;
        _options.DisableAutoAiming = DEFAULT_DISABLE_AUTO_AIMING;
        _options.ViewMode          = DEFAULT_VIEW_MODE;
    }

    void ConfigurationManager::SetDefaultInputSystemOptions()
    {
        _options.EnableToasts    = DEFAULT_ENABLE_TOASTS;
        _options.EnableDebugMode = IS_DEBUG_BUILD;
    }

    void ConfigurationManager::Initialize()
    {
        constexpr char APP_FOLDER_NAME[] = "Silent Engine";

        char*  buffer = nullptr;
        size_t length = 0;

        // Set application directory.
        switch (OS_TYPE)
        {
            case OsType::Windows:
            {
                // Get `APPDATA` path.
                if (_dupenv_s(&buffer, &length, "APPDATA") == 0 && buffer != nullptr)
                {
                    auto path = std::filesystem::path(buffer);
                    free(buffer);
                    _appDirPath = path / APP_FOLDER_NAME; 
                }
                break;
            }

            case OsType::MacOs:
            {
                // Get `HOME` path.
                if (_dupenv_s(&buffer, &length, "HOME") == 0 && buffer != nullptr)
                {
                    auto path = std::filesystem::path(buffer);
                    free(buffer);
                    _appDirPath = path / APP_FOLDER_NAME; 
                }
                break;
            }

            case OsType::Linux:
            {
                // Get `HOME` path.
                if (_dupenv_s(&buffer, &length, "HOME") == 0 && buffer != nullptr)
                {
                    auto path = std::filesystem::path(buffer);
                    free(buffer);
                    _appDirPath = path / APP_FOLDER_NAME; 
                }
                break;
            }

            default:
            {
                break;
            }
        }
        Assert(!_appDirPath.empty(), "Failed to initialize `ConfigurationManager`.");

        LoadOptions();
    }

    void ConfigurationManager::SaveOptions()
    {
        // Create options JSON.
        auto optionsJson = ToOptionsJson(_options);

        // Ensure directory exists.
        auto path = GetAppDirPath() / OPTIONS_FILE_PATH;
        if (std::filesystem::create_directories(path.parent_path()))
        {
            Log("Created path " + path.string() + ".");
        }

        // Write options JSON file.
        auto outputFile = std::ofstream(path);
        if (outputFile.is_open())
        {
            outputFile << optionsJson.dump(JSON_INDENT_SIZE);
            outputFile.close();
        }
    }

    void ConfigurationManager::LoadOptions()
    {
        auto path = GetAppDirPath() / OPTIONS_FILE_PATH;
        
        // Open options JSON file.
        auto inputFile = std::ifstream(path);
        if (!inputFile.is_open())
        {
            Log("No options file found. Creating file.", LogLevel::Info);

            SetDefaultOptions();
            SaveOptions();
            return;
        }

        // Parse file into JSON object.
        auto optionsJson = json();
        inputFile >> optionsJson;

        // Read options JSON.
        _options = FromOptionsJson(optionsJson);
    }

    void ConfigurationManager::SetDefaultOptions()
    {
        SetDefaultGraphicsOptions();
        SetDefaultGameplayOptions();

        SetDefaultInputKmBindingsOptions();
        _options.ActiveKeyboardMouseProfileId = DEFAULT_ACTIVE_KEYBOARD_MOUSE_BINDING_PROFILE_ID;

        SetDefaultInputGamepadCustomBindingOptions();
        _options.ActiveGamepadProfileId = DEFAULT_ACTIVE_GAMEPAD_BINDING_PROFILE_ID;

        SetDefaultInputControlsOptions();
        SetDefaultInputSystemOptions();
    }

    Options ConfigurationManager::FromOptionsJson(const json& optionsJson) const
    {
        auto options = Options{};

        // Load graphics options.
        const auto& graphicsJson   = optionsJson[KEY_GRAPHICS];
        options.WindowedSize.x     = graphicsJson.value(KEY_WINDOWED_SIZE_X,      DEFAULT_WINDOWED_SIZE.x);
        options.WindowedSize.y     = graphicsJson.value(KEY_WINDOWED_SIZE_Y,      DEFAULT_WINDOWED_SIZE.y);
        options.EnableMaximized    = graphicsJson.value(KEY_ENABLE_MAXIMIZED,     DEFAULT_ENABLE_MAXIMIZED);
        options.EnableFullscreen   = graphicsJson.value(KEY_ENABLE_FULLSCREEN,    DEFAULT_ENABLE_FULLSCREEN);
        options.BrightnessLevel    = graphicsJson.value(KEY_BRIGHTNESS_LEVEL,     DEFAULT_BRIGHTNESS_LEVEL);
        options.RenderScaleType    = graphicsJson.value(KEY_RENDER_SCALE_TYPE,    DEFAULT_RENDER_SCALE_TYPE);
        options.AspectRatioType    = graphicsJson.value(KEY_ASPECT_RATIO_TYPE,    DEFAULT_ASPECT_RATIO_TYPE);
        options.TextureFilterType  = graphicsJson.value(KEY_TEXTURE_FILTER_TYPE,  DEFAULT_TEXTURE_FILTER_TYPE);
        options.EnableDithering    = graphicsJson.value(KEY_ENABLE_DITHERING,     DEFAULT_ENABLE_DITHERING);
        options.EnableCrtFilter    = graphicsJson.value(KEY_ENABLE_CRT_FILTER,    DEFAULT_ENABLE_CRT_FILTER);
        options.EnableVertexJitter = graphicsJson.value(KEY_ENABLE_VERTEX_JITTER, DEFAULT_ENABLE_VERTEX_JITTER);

        // Load gameplay options.
        const auto& gameplayJson = optionsJson[KEY_GAMEPLAY];
        options.EnableAutoLoad   = gameplayJson.value(KEY_ENABLE_AUTO_LOAD, DEFAULT_ENABLE_AUTO_LOAD);
        options.EnableSubtitles  = gameplayJson.value(KEY_ENABLE_SUBTITLES, DEFAULT_ENABLE_SUBTITLES);
        options.SoundType        = gameplayJson.value(KEY_SOUND_TYPE,       DEFAULT_SOUND_TYPE);
        options.BgmVolume        = gameplayJson.value(KEY_BGM_VOLUME,       DEFAULT_BGM_VOLUME);
        options.SeVolume         = gameplayJson.value(KEY_SE_VOLUME,        DEFAULT_SE_VOLUME);
        options.BloodColor       = gameplayJson.value(KEY_BLOOD_COLOR,      DEFAULT_BLOOD_COLOR);
        options.BulletAdjust     = gameplayJson.value(KEY_BULLET_ADJUST,    DEFAULT_BULLET_ADJUST);

        // Load input options.
        const auto& inputJson                = optionsJson[KEY_INPUT];
        options.ActiveKeyboardMouseProfileId = inputJson.value(KEY_ACTIVE_KEYBOARD_MOUSE_BINDING_PROFILE_ID, DEFAULT_ACTIVE_KEYBOARD_MOUSE_BINDING_PROFILE_ID);
        options.ActiveGamepadProfileId       = inputJson.value(KEY_ACTIVE_GAMEPAD_BINDING_PROFILE_ID,        DEFAULT_ACTIVE_GAMEPAD_BINDING_PROFILE_ID);
        options.EnableVibration              = inputJson.value(KEY_ENABLE_VIBRATION,                         DEFAULT_ENABLE_VIBRATION);
        options.MouseSensitivity             = inputJson.value(KEY_MOUSE_SENSITIVITY,                        DEFAULT_MOUSE_SENSITIVITY);
        options.WeaponControl                = inputJson.value(KEY_WEAPON_CONTROL,                           DEFAULT_WEAPON_CONTROL);
        options.ViewControl                  = inputJson.value(KEY_VIEW_CONTROL,                             DEFAULT_VIEW_CONTROL);
        options.RetreatTurn                  = inputJson.value(KEY_RETREAT_TURN,                             DEFAULT_RETREAT_TURN);
        options.WalkRunControl               = inputJson.value(KEY_WALK_RUN_CONTROL,                         DEFAULT_WALK_RUN_CONTROL);
        options.DisableAutoAiming            = inputJson.value(KEY_DISABLE_AUTO_AIMING,                      DEFAULT_DISABLE_AUTO_AIMING);
        options.ViewMode                     = inputJson.value(KEY_VIEW_MODE,                                DEFAULT_VIEW_MODE);

        // Load custom action-event bindings.
        const auto& kmBindsJson      = inputJson[KEY_KEYBOARD_MOUSE_BINDINGS];
        const auto& gamepadBindsJson = inputJson[KEY_GAMEPAD_BINDINGS];
        for (auto actionGroupId : USER_ACTION_GROUP_IDS)
        {
            const auto& actionIds = ACTION_ID_GROUPS.at(actionGroupId);
            for (auto actionId : actionIds)
            {
                auto actionStr = std::to_string((int)actionId);

                // Keyboard/mouse.
                if (kmBindsJson.contains(actionStr))
                {
                    const auto& eventsJson = kmBindsJson[actionStr];

                    auto events = std::vector<EventId>{};
                    events.reserve(eventsJson.size());
                    for (const auto& eventJson : eventsJson)
                    {
                        events.push_back((EventId)eventJson.get<int>());
                    }

                    options.KeyboardMouseBindings[actionId] = !events.empty() ? std::move(events) : USER_KEYBOARD_MOUSE_BINDING_PROFILE_TYPE_1.at(actionId);
                }
                else
                {
                    options.KeyboardMouseBindings[actionId] = USER_KEYBOARD_MOUSE_BINDING_PROFILE_TYPE_1.at(actionId);
                }

                // Gamepad.
                if (gamepadBindsJson.contains(actionStr))
                {
                    const auto& eventsJson = gamepadBindsJson[actionStr];

                    auto events = std::vector<EventId>{};
                    events.reserve(eventsJson.size());
                    for (const auto& eventJson : eventsJson)
                    {
                        events.push_back((EventId)eventJson.get<int>());
                    }

                    options.GamepadBindings[actionId] = !events.empty() ? std::move(events) : USER_GAMEPAD_BINDING_PROFILE_TYPE_1.at(actionId);
                }
                else
                {
                    options.GamepadBindings[actionId] = USER_GAMEPAD_BINDING_PROFILE_TYPE_1.at(actionId);
                }
            }
        }

        // Load system options.
        const auto& systemJson = optionsJson[KEY_SYSTEM];
        options.EnableToasts   = systemJson.value(KEY_ENABLE_TOASTS, DEFAULT_ENABLE_TOASTS);

        return options;
    }

    json ConfigurationManager::ToOptionsJson(const Options& options) const
    {
        // Create keyboard/mouse action-event bindings JSON.
        auto kmBindsJson = json();
        for (const auto& [actionId, eventIds] : options.KeyboardMouseBindings)
        {
            auto eventsJson = json::array();
            for (const auto& eventId : eventIds)
            {
                eventsJson.push_back(eventId);
            }

            kmBindsJson[std::to_string((int)actionId)] = eventsJson;
        }

        // Create gamepad action-event bindings JSON.
        auto gamepadBindsJson = json();
        for (const auto& [actionId, eventIds] : options.KeyboardMouseBindings)
        {
            auto eventsJson = json::array();
            for (const auto& eventId : eventIds)
            {
                eventsJson.push_back(eventId);
            }

            gamepadBindsJson[std::to_string((int)actionId)] = eventsJson;
        }

        // Create options JSON.
        return json
        {
            {
                KEY_GRAPHICS,
                {
                    { KEY_WINDOWED_SIZE_X,      options.WindowedSize.x },
                    { KEY_WINDOWED_SIZE_Y,      options.WindowedSize.y },
                    { KEY_ENABLE_MAXIMIZED,     options.EnableMaximized },
                    { KEY_ENABLE_FULLSCREEN,    options.EnableFullscreen },
                    { KEY_BRIGHTNESS_LEVEL,     options.BrightnessLevel },
                    { KEY_RENDER_SCALE_TYPE,    options.RenderScaleType },
                    { KEY_ASPECT_RATIO_TYPE,    options.AspectRatioType },
                    { KEY_TEXTURE_FILTER_TYPE,  options.TextureFilterType },
                    { KEY_ENABLE_DITHERING,     options.EnableDithering },
                    { KEY_ENABLE_CRT_FILTER,    options.EnableCrtFilter },
                    { KEY_ENABLE_VERTEX_JITTER, options.EnableVertexJitter }
                }
            },
            {
                KEY_GAMEPLAY,
                {
                    { KEY_ENABLE_AUTO_LOAD, options.EnableAutoLoad },
                    { KEY_ENABLE_SUBTITLES, options.EnableSubtitles },
                    { KEY_SOUND_TYPE,       options.SoundType },
                    { KEY_BGM_VOLUME,       options.BgmVolume },
                    { KEY_SE_VOLUME,        options.SeVolume },
                    { KEY_BLOOD_COLOR,      options.BloodColor },
                    { KEY_BULLET_ADJUST,    options.BulletAdjust }
                }
            },
            {
                KEY_INPUT,
                {
                    { KEY_KEYBOARD_MOUSE_BINDINGS,                  kmBindsJson },
                    { KEY_ACTIVE_KEYBOARD_MOUSE_BINDING_PROFILE_ID, options.ActiveKeyboardMouseProfileId },
                    { KEY_GAMEPAD_BINDINGS,                         gamepadBindsJson },
                    { KEY_ACTIVE_GAMEPAD_BINDING_PROFILE_ID,        options.ActiveGamepadProfileId },
                    { KEY_ENABLE_VIBRATION,                         options.EnableVibration },
                    { KEY_MOUSE_SENSITIVITY,                        options.MouseSensitivity },
                    { KEY_WEAPON_CONTROL,                           options.WeaponControl },
                    { KEY_VIEW_CONTROL,                             options.ViewControl },
                    { KEY_RETREAT_TURN,                             options.RetreatTurn },
                    { KEY_WALK_RUN_CONTROL,                         options.WalkRunControl },
                    { KEY_DISABLE_AUTO_AIMING,                      options.DisableAutoAiming },
                    { KEY_VIEW_MODE,                                options.ViewMode }
                }
            },
            {
                KEY_SYSTEM,
                {
                    { KEY_ENABLE_TOASTS, options.EnableToasts }
                }
            }
        };
    }
}
