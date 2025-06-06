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

    constexpr char APP_NAME[]                       = "Silent Engine";
    constexpr char APP_VERSION[]                    = "0.1.0";

#if defined(_WIN32) || defined(_WIN64)
    constexpr auto OS_TYPE = OsType::Windows;
#elif defined(__APPLE__)
    constexpr auto OS_TYPE = OsType::MacOs;
#elif defined(__linux__)
    constexpr auto OS_TYPE = OsType::Linux;
#endif

    constexpr char ASSETS_FOLDER_NAME[]                  = "Assets";
    constexpr char SAVEGAME_SLOT_FILE_FOLDER_NAME_BASE[] = "File ";
    constexpr char SAVEGAME_FOLDER_NAME[]                = "Savegame";
    constexpr char SCREENSHOTS_FOLDER_NAME[]             = "Screenshots";
    constexpr char SHADERS_FOLDER_NAME[]                 = "Shaders";
    constexpr char SAVEGAME_SLOT_FOLDER_NAME_BASE[]      = "Slot ";

    constexpr char SCREENSHOT_FILENAME_BASE[] = "Screenshot_";
    constexpr char OPTIONS_FILENAME[]         = "Options";
    
    constexpr char JSON_FILE_EXT[]     = ".json";
    constexpr char PNG_FILE_EXT[]      = ".png";
    constexpr char SAVEGAME_FILE_EXT[] = ".savegame";
    
    constexpr uint JSON_INDENT_SIZE = 4;

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

    enum class TextureFilterType
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

        bool              EnableFullscreen   = false;
        int               BrightnessLevel    = 0;
        FrameRateType     FrameRateType      = FrameRateType::Fps30;
        RenderScaleType   RenderScaleType    = RenderScaleType::Native;
        AspectRatioType   AspectRatioType    = AspectRatioType::Native;
        TextureFilterType TextureFilterType  = TextureFilterType::Nearest;
        bool              EnableDithering    = false;
        bool              EnableCrtFilter    = false;
        bool              EnableVertexJitter = false;

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

        bool EnableToasts      = false; // Popup messages, e.g. "Gamepad connected", "Gamepad disconnected", etc.
        bool EnableParallelism = false;
        bool EnableDebugMode   = true; // TODO: Can't depend on `IS_BUILD_MODE` in init func, have to set this here for some reason.
        bool EnableDebugGui    = false;
    };

    class ConfigurationManager
    {
    private:
        // Fields

        std::filesystem::path _appFolder         = {};
        std::filesystem::path _assetsFolder      = {};
        std::filesystem::path _workFolder        = {};
        std::filesystem::path _screenshotsFolder = {};
        Options               _options           = {};

    public:
        // Constructors

        ConfigurationManager() = default;

        // Getters

        const std::filesystem::path& GetAppFolder() const;
        const std::filesystem::path& GetAssetsFolder() const;
        const std::filesystem::path& GetWorkFolder() const;
        const std::filesystem::path& GetScreenshotsFolder() const;
        Options&                     GetOptions();

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
