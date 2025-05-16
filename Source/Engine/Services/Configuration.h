#pragma once

#include "Engine/Input/Binding.h"

using namespace Silent::Input;

namespace Silent::Services
{
    enum class OsType
    {
        Windows,
        MacOs,
        Linux
    };

    struct Options
    {
        static constexpr bool DEFAULT_IS_FULLSCREEN     = false;
        static constexpr auto DEFAULT_SCREEN_RESOLUTION = Vector2i(800, 600);

        static constexpr int  DEFAULT_BRIGHTNESS_LEVEL    = 3;
        static constexpr bool DEFAULT_ENABLE_VIBRATION    = true;
        static constexpr bool DEFAULT_ENABLE_AUTO_LOAD    = false;
        static constexpr int  DEFAULT_SOUND_TYPE          = 0;
        static constexpr int  DEFAULT_BGM_VOLUME          = 16;
        static constexpr int  DEFAULT_SE_VOLUME           = 16;
        static constexpr int  DEFAULT_WEAPON_CONTROL      = 1;
        static constexpr int  DEFAULT_BLOOD_COLOR         = 0;
        static constexpr int  DEFAULT_VIEW_CONTROL        = 0;
        static constexpr int  DEFAULT_RETREAT_TURN        = 0;
        static constexpr int  DEFAULT_WALK_RUN_CONTROL    = 0;
        static constexpr bool DEFAULT_DISABLE_AUTO_AIMING = false;
        static constexpr int  DEFAULT_VIEW_MODE           = 0;
        static constexpr int  DEFAULT_BULLET_ADJUST       = 0;

        static constexpr auto DEFAULT_ACTIVE_KEYBOARD_MOUSE_BINDING_PROFILE_ID = BindingProfileId::DefaultKeyboardMouse0;
        static constexpr auto DEFAULT_ACTIVE_GAMEPAD_BINDING_PROFILE_ID        = BindingProfileId::DefaultGamepad0;
        static constexpr int  DEFAULT_MOUSE_SENSITIVITY                        = 6;

        // Graphics

        bool     IsFullscreen     = false;
        Vector2i ScreenResolution = Vector2i::Zero;

        // Gameplay

        int  BrightnessLevel   = 0;
        bool EnableVibration   = false;
        bool EnableAutoLoad    = false;
        int  SoundType         = 0;
        int  BgmVolume         = 0;
        int  SeVolume          = 0;
        int  WeaponControl     = 0;
        int  BloodColor        = 0;
        int  ViewControl       = 0;
        int  RetreatTurn       = 0;
        int  WalkRunControl    = 0;
        bool DisableAutoAiming = false;
        int  ViewMode          = 0;
        int  BulletAdjust      = 0;

        // Input

        BindingProfile   KeyboardMouseBindings        = {};
        BindingProfile   GamepadBindings              = {};
        BindingProfileId ActiveKeyboardMouseProfileId = BindingProfileId::CustomKeyboardMouse;
        BindingProfileId ActiveGamepadProfileId       = BindingProfileId::CustomKeyboardMouse;
        int              MouseSensitivity             = 0;
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
        static constexpr char OPTIONS_PATH[]    = "Options.json";

        // Fields

        std::filesystem::path _appDir  = {};
        Options               _options = {};

    public:
        // Constructors

        ConfigurationManager() = default;

        // Getters

        std::filesystem::path GetAppDir() const;
        Options&              GetOptions();

        // Utilities

        void Initialize();
        void SaveOptions();
        void LoadOptions();

    private:
        // Helpers

        void SetDefaultOptions();
    };

    extern ConfigurationManager g_Config;
}
