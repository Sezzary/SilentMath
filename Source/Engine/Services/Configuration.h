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
        // Graphics

        Vector2i             WindowSize         = Vector2i::Zero;
        bool                 EnableFullscreen   = false;
        int                  BrightnessLevel    = 0;
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

        // Input keyboard/mouse bindings

        BindingProfile   KeyboardMouseBindings        = {};
        BindingProfileId ActiveKeyboardMouseProfileId = BindingProfileId::KeyboardMouseType1;

        // Input gamepad bindings

        BindingProfile   GamepadBindings        = {};
        BindingProfileId ActiveGamepadProfileId = BindingProfileId::GamepadType1;

        // Input controls

        bool EnableVibration   = false;
        int  MouseSensitivity  = 0;
        int  WeaponControl     = 0;
        int  ViewControl       = 0;
        int  RetreatTurn       = 0;
        int  WalkRunControl    = 0;
        bool DisableAutoAiming = false;
        int  ViewMode          = 0;
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

        static constexpr int JSON_INDENT_SIZE = 4;

    private:
        static constexpr char OPTIONS_PATH[] = "Options.json";

        // Fields

        std::filesystem::path _appDir  = {};
        Options               _options = {};

    public:
        // Constructors

        ConfigurationManager() = default;

        // Getters

        std::filesystem::path GetAppDir() const;
        Options&              GetOptions();

        // Setters

        void SetDefaultGraphicsOptions();
        void SetDefaultGameplayOptions();
        void SetDefaultInputKeyboardMouseCustomBindingOptions();
        void SetDefaultInputGamepadCustomBindingOptions();
        void SetDefaultInputControlOptions();

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
