#pragma once

#include "Engine/Input/Action.h"
#include "Engine/Input/Events.h"

namespace Silent::Input
{
    using BindingProfile = std::unordered_map<ActionId, std::vector<EventId>>; // Key = action ID, value = event IDs.

    extern const BindingProfile DEFAULT_USER_KEYBOARD_MOUSE_BINDING_PROFILE_0;
    extern const BindingProfile DEFAULT_USER_KEYBOARD_MOUSE_BINDING_PROFILE_1;
    extern const BindingProfile DEFAULT_USER_KEYBOARD_MOUSE_BINDING_PROFILE_2;

    extern const BindingProfile DEFAULT_USER_GAMEPAD_BINDING_PROFILE_0;
    extern const BindingProfile DEFAULT_USER_GAMEPAD_BINDING_PROFILE_1;
    extern const BindingProfile DEFAULT_USER_GAMEPAD_BINDING_PROFILE_2;

    extern const BindingProfile RAW_KEYBOARD_BINDING_PROFILE;
    extern const BindingProfile RAW_MOUSE_BINDING_PROFILE;
    extern const BindingProfile RAW_GAMEPAD_BINDING_PROFILE;

    enum class BindingProfileId
    {
        // Keyboard/mouse

        CustomKeyboardMouse,
        DefaultKeyboardMouse0,
        DefaultKeyboardMouse1,
        DefaultKeyboardMouse2,

        // Gamepad

        CustomGamepad,
        DefaultGamepad0,
        DefaultGamepad1,
        DefaultGamepad2,

        // Raw

        RawKeyboard,
        RawMouse,
        RawGamepad,

        Count
    };

    extern const std::vector<BindingProfileId> USER_KEYBOARD_MOUSE_BINDING_PROFILE_IDS;
    extern const std::vector<BindingProfileId> USER_GAMEPAD_BINDING_PROFILE_IDS;
    extern const std::vector<BindingProfileId> RAW_EVENT_BINDING_PROFILE_IDS;

    class BindingManager
    {
    private:
        // Fields

        std::unordered_map<BindingProfileId, BindingProfile> _bindings  = {}; // Key = binding profile ID, value = binding profile.
        std::unordered_map<ActionId, bool>                   _conflicts = {}; // Key = action ID, value = has conflict.

    public:
        // Constructors

        BindingManager() = default;

        // Getters

        // TODO
        // Split kb/m and gamepad bindings. Get one or the other. as default, always fall back on binding 0 for each.
        
        const std::vector<EventId>& GetBoundEventIds(BindingProfileId profileId, ActionId actionId) const;
        const BindingProfile&       GetBindingProfile(BindingProfileId profileId) const;

        // Setters

        void SetEventBinding(BindingProfileId profileId, ActionId actionId, EventId eventId);
        void SetConflict(ActionId actionId, bool hasConflict);

        // Inquirers

        bool TestConflict(ActionId actionId) const;

        // Utilities

        void Initialize(const BindingProfile& customKeyboardMouseBindings, const BindingProfile& customGamepadBindings);
    };
}
