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
    constexpr char KEY_WINDOW_SIZE_X[]                            = "WindowSizeX";
    constexpr char KEY_WINDOW_SIZE_Y[]                            = "WindowSizeY";
    constexpr char KEY_ENABLE_FULLSCREEN[]                        = "EnableFullscreen";
    constexpr char KEY_BRIGHTNESS_LEVEL[]                         = "BrightnessLevel";
    constexpr char KEY_TEXTURE_FILTER_TYPE[]                      = "TextureFilterType";
    constexpr char KEY_ENABLE_DITHERING[]                         = "EnableDithering";
    constexpr char KEY_ENABLE_CRT_FILTER[]                        = "EnableCrtFilter";
    constexpr char KEY_ENABLE_VERTEX_JITTER[]                     = "EnableVertexJitter";
    constexpr char KEY_RENDER_SCALE_TYPE[]                        = "RenderScaleType";
    constexpr char KEY_ASPECT_RATIO_TYPE[]                        = "AspectRatioType";
    constexpr char KEY_ENABLE_VIBRATION[]                         = "EnableVibration";
    constexpr char KEY_ENABLE_AUTO_LOAD[]                         = "EnableAutoLoad";
    constexpr char KEY_SOUND_TYPE[]                               = "SoundType";
    constexpr char KEY_BGM_VOLUME[]                               = "BgmVolume";
    constexpr char KEY_SE_VOLUME[]                                = "SeVolume";
    constexpr char KEY_WEAPON_CONTROL[]                           = "WeaponControl";
    constexpr char KEY_BLOOD_COLOR[]                              = "BloodColor";
    constexpr char KEY_VIEW_CONTROL[]                             = "ViewControl";
    constexpr char KEY_RETREAT_TURN[]                             = "RetreatTurn";
    constexpr char KEY_WALK_RUN_CONTROL[]                         = "WalkRunControl";
    constexpr char KEY_DISABLE_AUTO_AIMING[]                      = "DisableAutoAiming";
    constexpr char KEY_VIEW_MODE[]                                = "ViewMode";
    constexpr char KEY_BULLET_ADJUST[]                            = "BulletAdjust";
    constexpr char KEY_KEYBOARD_MOUSE_BINDINGS[]                  = "KeyboardMouseBindings";
    constexpr char KEY_ACTIVE_KEYBOARD_MOUSE_BINDING_PROFILE_ID[] = "ActiveKeyboardMouseProfileId";
    constexpr char KEY_GAMEPAD_BINDINGS[]                         = "GamepadBindings";  
    constexpr char KEY_ACTIVE_GAMEPAD_BINDING_PROFILE_ID[]        = "ActiveGamepadProfileId";
    constexpr char KEY_MOUSE_SENSITIVITY[]                        = "MouseSensitivity";

    constexpr auto DEFAULT_WINDOW_SIZE                              = Vector2i(800, 600);
    constexpr bool DEFAULT_ENABLE_FULLSCREEN                        = false;
    constexpr int  DEFAULT_BRIGHTNESS_LEVEL                         = 3;
    constexpr auto DEFAULT_RENDER_SCALE_TYPE                        = RenderScaleType::Native;
    constexpr auto DEFAULT_ASPECT_RATIO_TYPE                        = AspectRatioType::Native;
    constexpr auto DEFAULT_TEXTURE_FILTER_TYPE                      = TextureFilteringType::Nearest;
    constexpr bool DEFAULT_ENABLE_DITHERING                         = true;
    constexpr bool DEFAULT_ENABLE_CRT_FILTER                        = false;
    constexpr bool DEFAULT_ENABLE_VERTEX_JITTER                     = false;
    constexpr bool DEFAULT_ENABLE_VIBRATION                         = true;
    constexpr bool DEFAULT_ENABLE_AUTO_LOAD                         = false;
    constexpr int  DEFAULT_SOUND_TYPE                               = 0;
    constexpr int  DEFAULT_BGM_VOLUME                               = 16;
    constexpr int  DEFAULT_SE_VOLUME                                = 16;
    constexpr int  DEFAULT_WEAPON_CONTROL                           = 1;
    constexpr int  DEFAULT_BLOOD_COLOR                              = 0;
    constexpr int  DEFAULT_VIEW_CONTROL                             = 0;
    constexpr int  DEFAULT_RETREAT_TURN                             = 0;
    constexpr int  DEFAULT_WALK_RUN_CONTROL                         = 0;
    constexpr bool DEFAULT_DISABLE_AUTO_AIMING                      = false;
    constexpr int  DEFAULT_VIEW_MODE                                = 0;
    constexpr int  DEFAULT_BULLET_ADJUST                            = 0;
    constexpr auto DEFAULT_ACTIVE_KEYBOARD_MOUSE_BINDING_PROFILE_ID = BindingProfileId::DefaultKeyboardMouse0;
    constexpr auto DEFAULT_ACTIVE_GAMEPAD_BINDING_PROFILE_ID        = BindingProfileId::DefaultGamepad0;
    constexpr int  DEFAULT_MOUSE_SENSITIVITY                        = 6;

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
        constexpr char APP_FOLDER_NAME[] = "Silent Engine";

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
        // Collect user keyboard/mouse action-event bindings.
        auto kbMouseBindsJson = json();
        for (const auto& [actionId, eventIds] : _options.KeyboardMouseBindings)
        {
            auto eventsJson = json::array();
            for (const auto& eventId : eventIds)
            {
                eventsJson.push_back(eventId);
            }

            kbMouseBindsJson[std::to_string((int)actionId)] = eventsJson;
        }

        // Collect user gamepad action-event bindings.
        auto gamepadBindsJson = json();
        for (const auto& [actionId, eventIds] : _options.KeyboardMouseBindings)
        {
            auto eventsJson = json::array();
            for (const auto& eventId : eventIds)
            {
                eventsJson.push_back(eventId);
            }

            gamepadBindsJson[std::to_string((int)actionId)] = eventsJson;
        }

        // Create options JSON.
        auto optionsJson = json
        {
            {
                KEY_GRAPHICS,
                {
                    { KEY_WINDOW_SIZE_X,        _options.WindowSize.x },
                    { KEY_WINDOW_SIZE_Y,        _options.WindowSize.y },
                    { KEY_ENABLE_FULLSCREEN,    _options.EnableFullscreen },
                    { KEY_BRIGHTNESS_LEVEL,     _options.BrightnessLevel },
                    { KEY_RENDER_SCALE_TYPE,    _options.RenderScaleType },
                    { KEY_ASPECT_RATIO_TYPE,    _options.AspectRatioType },
                    { KEY_TEXTURE_FILTER_TYPE,  _options.TextureFilterType },
                    { KEY_ENABLE_DITHERING,     _options.EnableDithering },
                    { KEY_ENABLE_CRT_FILTER,    _options.EnableCrtFilter },
                    { KEY_ENABLE_VERTEX_JITTER, _options.EnableVertexJitter }
                }
            },
            {
                KEY_GAMEPLAY,
                {
                    { KEY_ENABLE_VIBRATION,    _options.EnableVibration },
                    { KEY_ENABLE_AUTO_LOAD,    _options.EnableAutoLoad },
                    { KEY_SOUND_TYPE,          _options.SoundType },
                    { KEY_BGM_VOLUME,          _options.BgmVolume },
                    { KEY_SE_VOLUME,           _options.SeVolume },
                    { KEY_WEAPON_CONTROL,      _options.WeaponControl },
                    { KEY_BLOOD_COLOR,         _options.BloodColor },
                    { KEY_VIEW_CONTROL,        _options.ViewControl },
                    { KEY_RETREAT_TURN,        _options.RetreatTurn },
                    { KEY_WALK_RUN_CONTROL,    _options.WalkRunControl },
                    { KEY_DISABLE_AUTO_AIMING, _options.DisableAutoAiming },
                    { KEY_VIEW_MODE,           _options.ViewMode },
                    { KEY_BULLET_ADJUST,       _options.BulletAdjust }
                }
            },
            {
                KEY_INPUT,
                {
                    { KEY_KEYBOARD_MOUSE_BINDINGS,                  kbMouseBindsJson },
                    { KEY_ACTIVE_KEYBOARD_MOUSE_BINDING_PROFILE_ID, _options.ActiveKeyboardMouseProfileId },
                    { KEY_GAMEPAD_BINDINGS,                         gamepadBindsJson },
                    { KEY_ACTIVE_GAMEPAD_BINDING_PROFILE_ID,        _options.ActiveGamepadProfileId },
                    { KEY_MOUSE_SENSITIVITY,                        _options.MouseSensitivity }
                }
            }
        };

        // Ensure directory exists.
        auto path = GetAppDir() / OPTIONS_PATH;
        std::filesystem::create_directories(path.parent_path());

        // Write options file.
        auto outputFile = std::ofstream(path);
        if (outputFile.is_open())
        {
            outputFile << optionsJson.dump(JSON_INDENT_SIZE);
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
        const auto& graphicsJson    = optionsJson[KEY_GRAPHICS];
        _options.WindowSize.x       = graphicsJson.value(KEY_WINDOW_SIZE_X,        DEFAULT_WINDOW_SIZE.x);
        _options.WindowSize.y       = graphicsJson.value(KEY_WINDOW_SIZE_Y,        DEFAULT_WINDOW_SIZE.y);
        _options.EnableFullscreen   = graphicsJson.value(KEY_ENABLE_FULLSCREEN,    DEFAULT_ENABLE_FULLSCREEN);
        _options.BrightnessLevel    = graphicsJson.value(KEY_BRIGHTNESS_LEVEL,     DEFAULT_BRIGHTNESS_LEVEL);
        _options.RenderScaleType    = graphicsJson.value(KEY_RENDER_SCALE_TYPE,    DEFAULT_RENDER_SCALE_TYPE);
        _options.AspectRatioType    = graphicsJson.value(KEY_ASPECT_RATIO_TYPE,    DEFAULT_ASPECT_RATIO_TYPE);
        _options.TextureFilterType  = graphicsJson.value(KEY_TEXTURE_FILTER_TYPE,  DEFAULT_TEXTURE_FILTER_TYPE);
        _options.EnableDithering    = graphicsJson.value(KEY_ENABLE_DITHERING,     DEFAULT_ENABLE_DITHERING);
        _options.EnableCrtFilter    = graphicsJson.value(KEY_ENABLE_CRT_FILTER,    DEFAULT_ENABLE_CRT_FILTER);
        _options.EnableVertexJitter = graphicsJson.value(KEY_ENABLE_VERTEX_JITTER, DEFAULT_ENABLE_VERTEX_JITTER);

        // Load gameplay options.
        const auto& gameplayJson   = optionsJson[KEY_GAMEPLAY];
        _options.EnableVibration   = gameplayJson.value(KEY_ENABLE_VIBRATION,    DEFAULT_ENABLE_VIBRATION);
        _options.EnableAutoLoad    = gameplayJson.value(KEY_ENABLE_AUTO_LOAD,    DEFAULT_ENABLE_AUTO_LOAD);
        _options.SoundType         = gameplayJson.value(KEY_SOUND_TYPE,          DEFAULT_SOUND_TYPE);
        _options.BgmVolume         = gameplayJson.value(KEY_BGM_VOLUME,          DEFAULT_BGM_VOLUME);
        _options.SeVolume          = gameplayJson.value(KEY_SE_VOLUME,           DEFAULT_SE_VOLUME);
        _options.WeaponControl     = gameplayJson.value(KEY_WEAPON_CONTROL,      DEFAULT_WEAPON_CONTROL);
        _options.BloodColor        = gameplayJson.value(KEY_BLOOD_COLOR,         DEFAULT_BLOOD_COLOR);
        _options.ViewControl       = gameplayJson.value(KEY_VIEW_CONTROL,        DEFAULT_VIEW_CONTROL);
        _options.RetreatTurn       = gameplayJson.value(KEY_RETREAT_TURN,        DEFAULT_RETREAT_TURN);
        _options.WalkRunControl    = gameplayJson.value(KEY_WALK_RUN_CONTROL,    DEFAULT_WALK_RUN_CONTROL);
        _options.DisableAutoAiming = gameplayJson.value(KEY_DISABLE_AUTO_AIMING, DEFAULT_DISABLE_AUTO_AIMING);
        _options.ViewMode          = gameplayJson.value(KEY_VIEW_MODE,           DEFAULT_VIEW_MODE);
        _options.BulletAdjust      = gameplayJson.value(KEY_BULLET_ADJUST,       DEFAULT_BULLET_ADJUST);

        // Load input options.
        const auto& inputJson                 = optionsJson[KEY_INPUT];
        _options.ActiveKeyboardMouseProfileId = inputJson.value(KEY_ACTIVE_KEYBOARD_MOUSE_BINDING_PROFILE_ID, DEFAULT_ACTIVE_KEYBOARD_MOUSE_BINDING_PROFILE_ID);
        _options.ActiveGamepadProfileId       = inputJson.value(KEY_ACTIVE_GAMEPAD_BINDING_PROFILE_ID,        DEFAULT_ACTIVE_GAMEPAD_BINDING_PROFILE_ID);
        _options.MouseSensitivity             = inputJson.value(KEY_MOUSE_SENSITIVITY,                        DEFAULT_MOUSE_SENSITIVITY);

        // Load user action-event bindings.
        const auto& kbMouseBindsJson = inputJson[KEY_KEYBOARD_MOUSE_BINDINGS];
        const auto& gamepadBindsJson = inputJson[KEY_GAMEPAD_BINDINGS];
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

                    auto events = std::vector<EventId>{};
                    events.reserve(eventsJson.size());
                    for (const auto& eventJson : eventsJson)
                    {
                        events.push_back((EventId)eventJson.get<int>());
                    }

                    _options.KeyboardMouseBindings[actionId] = !events.empty() ? std::move(events) :
                                                                                 DEFAULT_USER_KEYBOARD_MOUSE_BINDING_PROFILE_0.at(actionId);
                }
                else
                {
                    _options.KeyboardMouseBindings[actionId] = DEFAULT_USER_KEYBOARD_MOUSE_BINDING_PROFILE_0.at(actionId);
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

                    _options.GamepadBindings[actionId] = !events.empty() ? std::move(events) :
                                                                           DEFAULT_USER_GAMEPAD_BINDING_PROFILE_0.at(actionId);
                }
                else
                {
                    _options.GamepadBindings[actionId] = DEFAULT_USER_GAMEPAD_BINDING_PROFILE_0.at(actionId);
                }
            }
        }
    }

    void ConfigurationManager::SetDefaultOptions()
    {
        SetDefaultGraphicsOptions();
        SetDefaultGameplayOptions();
        SetDefaultInputOptions();
    }

    void ConfigurationManager::SetDefaultGraphicsOptions()
    {
        _options.WindowSize         = DEFAULT_WINDOW_SIZE;
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
        _options.EnableVibration   = DEFAULT_ENABLE_VIBRATION;
        _options.EnableAutoLoad    = DEFAULT_ENABLE_AUTO_LOAD;
        _options.SoundType         = DEFAULT_SOUND_TYPE;
        _options.BgmVolume         = DEFAULT_BGM_VOLUME;
        _options.SeVolume          = DEFAULT_SE_VOLUME;
        _options.WeaponControl     = DEFAULT_WEAPON_CONTROL;
        _options.BloodColor        = DEFAULT_BLOOD_COLOR;
        _options.ViewControl       = DEFAULT_VIEW_CONTROL;
        _options.RetreatTurn       = DEFAULT_RETREAT_TURN;
        _options.WalkRunControl    = DEFAULT_WALK_RUN_CONTROL;
        _options.DisableAutoAiming = DEFAULT_DISABLE_AUTO_AIMING;
        _options.ViewMode          = DEFAULT_VIEW_MODE;
        _options.BulletAdjust      = DEFAULT_BULLET_ADJUST;
    }

    void ConfigurationManager::SetDefaultInputOptions()
    {
        _options.KeyboardMouseBindings        = DEFAULT_USER_KEYBOARD_MOUSE_BINDING_PROFILE_0;
        _options.ActiveKeyboardMouseProfileId = DEFAULT_ACTIVE_KEYBOARD_MOUSE_BINDING_PROFILE_ID;
        _options.GamepadBindings              = DEFAULT_USER_GAMEPAD_BINDING_PROFILE_0;
        _options.ActiveGamepadProfileId       = DEFAULT_ACTIVE_GAMEPAD_BINDING_PROFILE_ID;
        _options.MouseSensitivity             = DEFAULT_MOUSE_SENSITIVITY;
    }
}
