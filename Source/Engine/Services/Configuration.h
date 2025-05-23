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

    enum class FrameRateType
    {
        Fps30,
        Fps60,
        Fps120,
        Unlimited
    };

    enum class RenderScaleType
    {
        Native,
        Half,
        Quarter,
        Retro
    };

    enum class AspectRatioType
    {
        Native,
        Widescreen,
        Retro
    };

    enum class TextureFilteringType
    {
        Nearest,
        Bilinear
    };

    struct Options
    {
        // Graphics (internal)

        Vector2i WindowedSize    = Vector2i::Zero;
        bool     EnableMaximized = false;

        // Graphics (user)

        bool                 EnableFullscreen   = false;
        int                  BrightnessLevel    = 0;
        FrameRateType        FrameRateType      = FrameRateType::Fps30;
        RenderScaleType      RenderScaleType    = RenderScaleType::Native;
        AspectRatioType      AspectRatioType    = AspectRatioType::Native;
        TextureFilteringType TextureFilterType  = TextureFilteringType::Nearest;
        bool                 EnableDithering    = false;
        bool                 EnableCrtFilter    = false;
        bool                 EnableVertexJitter = false;

        // Gameplay

        bool EnableAutoLoad  = false;
        bool EnableSubtitles = false;
        int  SoundType       = 0;
        int  BgmVolume       = 0;
        int  SeVolume        = 0;
        int  BloodColor      = 0;
        int  BulletAdjust    = 0;

        // TOO: Enhancements options section? Would contain settings for things like bugfixes.

        // Input (keyboard/mouse bindings)

        BindingProfile   KeyboardMouseBindings        = {};
        BindingProfileId ActiveKeyboardMouseProfileId = BindingProfileId::KeyboardMouseType1;

        // Input (gamepad bindings)

        BindingProfile   GamepadBindings        = {};
        BindingProfileId ActiveGamepadProfileId = BindingProfileId::GamepadType1;

        // Input (controls)

        bool EnableVibration   = false;
        int  MouseSensitivity  = 0;
        int  WeaponControl     = 0;
        int  ViewControl       = 0;
        int  RetreatTurn       = 0;
        int  WalkRunControl    = 0;
        bool DisableAutoAiming = false;
        int  ViewMode          = 0;

        // System

        bool EnableToasts    = false; // Popup messages, e.g. "Gamepad connected", "Gamepad disconnected", etc.
        bool EnableDebugMode = true;  // TODO: Temporarily hard-set to `true`. Wouldn't be saved in config file. User would have to turn it on manually each session.
    };

    class ConfigurationManager
    {
    public:
        // Constants

        static constexpr char APP_NAME[]                  = "Silent Engine";
        static constexpr char ASSETS_FOLDER_NAME[]        = "Assets";
        static constexpr char SCREENSHOTS_FOLDER_NAME[]   = "Screenshots";
        static constexpr char SLOT_FOLDER_NAME_BASE[]     = "Slot ";
        static constexpr char OPTIONS_FILE_NAME[]         = "Options";
        static constexpr char SCREENSHOT_FILE_NAME_BASE[] = "Screenshot_";
        static constexpr char PNG_FILE_EXT[]              = ".png";
        static constexpr char JSON_FILE_EXT[]             = ".json";
        static constexpr int  JSON_INDENT_SIZE            = 4;

#if defined(_WIN32) || defined(_WIN64)
        static constexpr auto OS_TYPE = OsType::Windows;
#elif defined(__APPLE__)
        static constexpr auto OS_TYPE = OsType::MacOs;
#elif defined(__linux__)
        static constexpr auto OS_TYPE = OsType::Linux;
#endif

private:
        // Fields

        std::filesystem::path _workFolderPath        = {};
        std::filesystem::path _screenshotsFolderPath = {};
        Options               _options               = {};

    public:
        // Constructors

        ConfigurationManager() = default;

        // Getters

        std::filesystem::path GetWorkFolderPath() const;
        std::filesystem::path GetScreenshotsFolderPath() const;
        Options&              GetOptions();

        // Setters

        void SetDefaultGraphicsOptions();
        void SetDefaultGameplayOptions();
        void SetDefaultInputKmBindingsOptions();
        void SetDefaultInputGamepadCustomBindingOptions();
        void SetDefaultInputControlsOptions();
        void SetDefaultInputSystemOptions();

        // Utilities

        void Initialize();
        void SaveOptions();
        void LoadOptions();

    private:
        // Helpers

        void SetDefaultOptions();
        
        Options FromOptionsJson(const json& optionsJson) const;
        json    ToOptionsJson(const Options& options) const;
    };
}
