#pragma once

#include "Input/Input.h"

using namespace Silent::Input;

namespace Silent::Services
{
    constexpr int BRIGHTNESS_LEVEL_MAX  = 7;
    constexpr int SOUND_VOLUME_MAX      = 128;
    constexpr int BULLET_ADJUST_MIN     = 1;
    constexpr int BULLET_ADJUST_MAX     = 6;
    constexpr int MOUSE_SENSITIVITY_MAX = 20;

    enum class GraphicsPresetType
    {
        None,
        Original,
        OriginalPlus,
        Recommended,
        Max
    };

    enum class FrameRateType
    {
        Fps30,
        Fps60
    };

    enum class RenderScaleType
    {
        Original,
        DoubleOriginal,
        Native
    };

    enum class AspectRatioType
    {
        Ratio4to3,
        Ratio16to9,
        Native
    };

    enum class TextureFilterType
    {
        Nearest,
        Linear
    };

    enum class TextQualityType
    {
        Original,
        Smooth
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
        Normal,
        Green,
        Violet,
        Black
    };

    enum class ControlInversionType
    {
        Normal,
        Reverse
    };

    enum class WeaponControlType
    {
        Switch,
        Press
    };

    enum class ViewMode
    {
        Normal,
        SelfView
    };

    enum class PaperMapQuality
    {
        Original,
        Scalable,
    };

    enum class DialogPauseType
    {
        Original,
        Condensed
    };

    /** @brief User options configuration data. */
    struct Options
    {
        // ====================
        // Graphics (internal)
        // ====================

        Vector2i WindowedSize    = Vector2i::Zero;
        bool     EnableMaximized = false;

        // ================
        // Graphics (user)
        // ================

        GraphicsPresetType PresetType         = GraphicsPresetType::None;
        bool               EnableFullscreen   = false;
        int                BrightnessLevel    = 0;
        FrameRateType      FrameRate          = FrameRateType::Fps30;
        RenderScaleType    RenderScale        = RenderScaleType::Native;
        AspectRatioType    AspectRatio        = AspectRatioType::Native;
        TextureFilterType  TextureFilter      = TextureFilterType::Nearest;
        TextQualityType    TextQuality        = TextQualityType::Smooth;
        LightingType       Lighting           = LightingType::PerVertex;
        bool               EnableVertexJitter = false;
        bool               EnableDithering    = false;
        bool               EnableVignette     = false;
        bool               EnableCrtFilter    = false;

        // =========
        // Gameplay
        // =========

        bool           EnableAutoLoad  = false;
        bool           EnableSubtitles = false;
        std::string    Language        = {};
        SoundType      Sound           = SoundType::Stereo;
        int            BgmVolume       = 0;
        int            SeVolume        = 0;
        BloodColorType BloodColor      = BloodColorType::Normal;
        int            BulletAdjust    = 0;

        // =================
        // Input (bindings)
        // =================

        BindingProfileId ActiveKeyboardMouseProfileId = BindingProfileId::DefaultKeyboardMouseType1;
        BindingProfileId ActiveGamepadProfileId       = BindingProfileId::DefaultGamepadType1;
        BindingProfile   KeyboardMouseBindings        = {};
        BindingProfile   GamepadBindings              = {};

        // =================
        // Input (controls)
        // =================

        bool                 EnableVibration    = false;
        int                  MouseSensitivity   = 0;
        WeaponControlType    WeaponControl      = WeaponControlType::Switch;
        ControlInversionType ViewControl        = ControlInversionType::Normal;
        ControlInversionType RetreatTurnControl = ControlInversionType::Normal;
        ControlInversionType WalkRunControl     = ControlInversionType::Normal;
        bool                 DisableAutoAiming  = false;
        ViewMode             ViewMode           = ViewMode::Normal;

        // =============
        // Enhancements
        // =============

        PaperMapQuality PaperMap    = PaperMapQuality::Original;
        DialogPauseType DialogPause = DialogPauseType::Original;

        // ==================
        // System (internal)
        // ==================

        bool EnablePowerMode = Debug::IS_DEBUG_BUILD;

        // ==============
        // System (user)
        // ==============

        bool EnableToasts      = false;
        bool EnableParallelism = false;
    };

    /** @brief User options configuration manager. */
    class OptionsManager
    {
    private:
        // =======
        // Fields
        // =======

        Options _options = {}; /** Options configuration data. */

    public:
        // =============
        // Constructors
        // =============

        /** @brief Creates a default uninitialized instance. */
        OptionsManager() = default;

        // ========
        // Setters
        // ========

        /** @brief Sets all graphics options to defaults. */
        void SetDefaultGraphicsOptions();

        /** @brief Sets all gameplay options to defaults. */
        void SetDefaultGameplayOptions();

        /** @brief Sets all keyboard/mouse input binding options to defaults. */
        void SetDefaultInputCustomKmBindingsOptions();

        /** @brief Sets all gamepad input binding options to defaults. */
        void SetDefaultInputCustomGamepadBindingOptions();

        /** @brief Sets all control input options to defaults. */
        void SetDefaultInputControlsOptions();

        /** @brief Sets all enhancements options to defaults. */
        void SetDefaultEnhancementsOptions();

        /** @brief Sets all system options, excluding debug, to defaults. */
        void SetDefaultSystemOptions();

        // ==========
        // Utilities
        // ==========

        /** @brief Initializes the options configurations to startup defaults, taking the build mode into account. */
        void Initialize();

        /** @brief Saves the current options configuration to a JSON file on the platforms's workspace folder. */
        void Save();

        /** @brief Loads the options configuration from a JSON file on the platforms's workspace folder. */
        void Load();

        // ==========
        // Operators
        // ==========

        const Options* operator->() const;
        Options*       operator->();

    private:
        // ========
        // Helpers
        // ========

        /** @brief Resets all configuration options to defaults.
         *
         * @note Excludes debug options.
         */
        void SetDefaultOptions();

        /** @brief Parses a JSON containing the options configuration to an internal options object.
         *
         * @note By convention, custom input bindings contain one input event per input action.
         *
         * @param optionsJson Options JSON to parse.
         * @return Internal `Options` object.
         */
        Options FromOptionsJson(const json& optionsJson) const;

        /** @brief Parses an internal options object to a JSON containing the options configuration.
         *
         * @note By convention, custom input bindings contain one input event per input action.
         *
         * @param options Internal `Options` object to parse.
         * @return Options JSON.
         */
        json ToOptionsJson(const Options& options) const;
    };
}
