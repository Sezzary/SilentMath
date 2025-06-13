#pragma once

#include "Engine/Input/Binding.h"

using namespace Silent::Input;

namespace Silent::Services
{
    constexpr int SOUND_VOLUME_MAX      = 128;
    constexpr int BULLET_ADJUST_MIN     = 1;
    constexpr int BULLET_ADJUST_MAX     = 6;
    constexpr int MOUSE_SENSITIVITY_MAX = 20;

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

    enum class LightingType
    {
        PerVertex,
        PerPixel
    };

    enum class SoundType
    {
        Stereo,
        Monaural
    };

    enum class BloodColorType
    {
        Normal = 0,
        Green  = 2,
        Violet = 5,
        Black  = 13
    };

    enum class WeaponControlType
    {
        Switch,
        Press
    };

    enum class ViewControlType
    {
        Normal,
        SelfView
    };

    enum class ControlInversionType
    {
        Normal,
        Reverse
    };

    struct Options
    {
        // Graphics (internal)

        Vector2i WindowedSize    = Vector2i::Zero;
        bool     EnableMaximized = false;

        // Graphics (user)

        bool              EnableFullscreen   = false;
        int               BrightnessLevel    = 0;
        FrameRateType     FrameRate          = FrameRateType::Fps30;
        RenderScaleType   RenderScale        = RenderScaleType::Native;
        AspectRatioType   AspectRatio        = AspectRatioType::Native;
        TextureFilterType TextureFilter      = TextureFilterType::Nearest;
        LightingType      Lighting           = LightingType::PerVertex;
        bool              EnableDithering    = false;
        bool              EnableCrtFilter    = false;
        bool              EnableVertexJitter = false;

        // Gameplay

        bool           EnableAutoLoad  = false;
        bool           EnableSubtitles = false;
        SoundType      Sound           = SoundType::Stereo;
        int            BgmVolume       = 0;
        int            SeVolume        = 0;
        BloodColorType BloodColor      = BloodColorType::Normal;
        int            BulletAdjust    = 0;

        // TOO: Enhancements options section? Would contain settings for things like bugfixes.

        // Input (keyboard/mouse bindings)

        BindingProfile   KeyboardMouseBindings        = {};
        BindingProfileId ActiveKeyboardMouseProfileId = BindingProfileId::KeyboardMouseType1;

        // Input (gamepad bindings)

        BindingProfile   GamepadBindings        = {};
        BindingProfileId ActiveGamepadProfileId = BindingProfileId::GamepadType1;

        // Input (controls)

        bool              EnableVibration   = false;
        int               MouseSensitivity  = 0;
        WeaponControlType WeaponControl     = WeaponControlType::Switch;
        ViewControlType   ViewControl       = ViewControlType::Normal;
        int               RetreatTurn       = 0;
        int               WalkRunControl    = 0;
        bool              DisableAutoAiming = false;
        int               ViewMode          = 0;

        // System (internal)

        bool EnableDebugMode = true; // TODO: Can't depend on `IS_BUILD_MODE` in init func, have to set this here for some reason.
        bool EnableDebugGui  = false;

        // System (user)

        bool EnableToasts      = false; // Popup messages, e.g. "Gamepad connected", "Gamepad disconnected", etc.
        bool EnableParallelism = false;
    };

    class OptionsManager
    {
    private:
        // Fields

        Options _options = {};

    public:
        // Constructors

        OptionsManager() = default;

        // Setters

        void SetDefaultGraphicsOptions();
        void SetDefaultGameplayOptions();
        void SetDefaultInputKmBindingsOptions();
        void SetDefaultInputGamepadCustomBindingOptions();
        void SetDefaultInputControlsOptions();
        void SetDefaultSystemOptions();

        // Utilities

        void Initialize();
        void Save();
        void Load();

        // Operators

        const Options* operator->() const;
        Options*       operator->();

    private:
        // Helpers

        void SetDefaultOptions();
        
        Options FromOptionsJson(const json& optionsJson) const;
        json    ToOptionsJson(const Options& options) const;
    };
}
