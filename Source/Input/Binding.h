#pragma once

namespace Silent::Input
{
    enum class ActionId;
    enum class BindingProfileId;
    enum class EventId;

    using BindingProfile = std::unordered_map<ActionId, std::vector<EventId>>; /** Key = action ID, value = event IDs. */

    extern const BindingProfile                DEFAULT_USER_KEYBOARD_MOUSE_BINDING_PROFILE;
    extern const BindingProfile                DEFAULT_USER_GAMEPAD_BINDING_PROFILE;
    extern const std::vector<BindingProfileId> CUSTOM_BINDING_PROFILE_IDS;
    extern const std::vector<BindingProfileId> DEFAULT_PROFILE_IDS;
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

        std::unordered_map<BindingProfileId, BindingProfile> _bindings = {}; /** Key = binding profile ID, value = binding profile. */

    public:
        // =============
        // Constructors
        // =============

        BindingManager() = default;

        // ========
        // Getters
        // ========

        /** @brief Gets the input event IDs bound to an input action within a binding profile.
         *
         * @param profileID Input binding profile ID.
         * @param actionID Input action ID.
         * @return `std::vector` containing the input event IDs bound to the given input action.
         */
        const std::vector<EventId>& GetBoundEventIds(BindingProfileId profileId, ActionId actionId) const;

        /** @brief Gets a reference to an input binding profile containing associations between input events and input actions.
         *
         * @param profileId Input binding profile ID to retrieve.
         * @return Input binding profile reference.
         */
        const BindingProfile& GetProfile(BindingProfileId profileId) const;

        // @todo Set bindings.

        // ==========
        // Utilities
        // ==========

        /** @brief Initializes the input binding manager, setting default bindings and active user bindings.
         *
         * @param customKeyboardMouseBinds Active keyboard/mouse input bindings to set for the user.
         * @param customGamepadBinds Active gamepoad input bindings to set for the user.
         */
        void Initialize(const BindingProfile& customKeyboardMouseBinds, const BindingProfile& customGamepadBinds);

        /** @brief Binds an input event to an input action, storing the new association inside an input binding profile.
         *
         * @param profileId Input binding profile ID to update.
         * @param actionId Input action ID to create a new binding for.
         * @param eventId Input event ID to bind to the input action.
         */
        void BindEventId(BindingProfileId profileId, ActionId actionId, EventId eventId);
    };
}
