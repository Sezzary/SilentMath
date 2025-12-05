#pragma once

namespace Silent::Input
{
    enum class ActionId;
    enum class BindingProfileId;
    enum class EventId;

    using BindingProfile = std::unordered_map<ActionId, std::vector<EventId>>; /** Key = action ID, value = event IDs. */

    extern const BindingProfile                DEFAULT_CUSTOM_KEYBOARD_MOUSE_BINDING_PROFILE;
    extern const BindingProfile                DEFAULT_CUSTOM_GAMEPAD_BINDING_PROFILE;
    extern const std::vector<BindingProfileId> CUSTOM_BINDING_PROFILE_IDS;
    extern const std::vector<BindingProfileId> DEFAULT_PROFILE_IDS;
    extern const std::vector<BindingProfileId> CUSTOM_BINDING_PROFILE_IDS;
    extern const std::vector<BindingProfileId> RAW_BINDING_PROFILE_IDS;

    /** @brief Input binding profile IDs. */
    enum class BindingProfileId
    {
        /* Custom */

        CustomKeyboardMouse,
        CustomGamepad,

        /** Default */

        DefaultKeyboardMouseType1,
        DefaultKeyboardMouseType2,
        DefaultKeyboardMouseType3,
        DefaultGamepadType1,
        DefaultGamepadType2,
        DefaultGamepadType3,

        /** Raw */

        RawKeyboard,
        RawMouse,
        RawGamepad,

        Count
    };

    /** @brief Input binder. */
    class BindingManager
    {
    private:
        // =======
        // Fields
        // =======

        std::vector<BindingProfile> _bindings = {}; /** Index = `BindingProfileId`. */

    public:
        // =============
        // Constructors
        // =============

        BindingManager() = default;

        // ========
        // Getters
        // ========

        /** @brief Gets an input binding profile containing associations between input events and input actions.
         *
         * @param profileId Input binding profile ID to retrieve.
         * @return Input binding profile.
         */
        const BindingProfile& GetProfile(BindingProfileId profileId) const;

        // ========
        // Setters
        // ========

        /** @brief Sets all bindings in an input binding profile to a new input binding profile.
         *
         * @param profileId ID of the input binding profile to update.
         * @param newProfile New input binding profile to set.
         */
        void SetProfile(BindingProfileId profileId, const BindingProfile& newProfile);

        /** @brief Sets a custom input event binding for an input action, storing the new association in an input binding profile.
         *
         * @param profileId ID of the input binding profile to update.
         * @param actionId ID of the input action to create a new binding for.
         * @param eventId ID of the input event to bind to the input action.
         */
        void SetBinding(BindingProfileId profileId, ActionId actionId, EventId eventId);

        // ==========
        // Utilities
        // ==========

        /** @brief Initializes the input binding manager with custom, default, and raw input binding profiles.
         *
         * @param customKeyboardMouseProfile Custom user input binding profile for keyboard/mouse.
         * @param customGamepadProfile Custom user input binding profile for gamepad.
         */
        void Initialize(const BindingProfile& customKeyboardMouseProfile, const BindingProfile& customGamepadProfile);
    };
}
