#pragma once

#include "Engine/Input/Action.h"
#include "Engine/Input/Events.h"

namespace Silent::Input
{
    using BindingProfile = std::unordered_map<ActionId, EventId>; // Key = action ID, value = event ID.

	enum class BindingProfileId
	{
		KeyboardMouseDefault0,
		KeyboardMouseDefault1,
		KeyboardMouseDefault2,
		KeyboardMouseCustom,

        GamepadDefault0,
        GamepadDefault1,
        GamepadDefault2,
        GamepadCustom,

		Count
	};

    class BindingManager
    {
    private:
        // Constants

        static const BindingProfile DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE_0;
        static const BindingProfile DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE_1;
        static const BindingProfile DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE_2;
        static const BindingProfile DEFAULT_GAMEPAD_BINDING_PROFILE_0;
        static const BindingProfile DEFAULT_GAMEPAD_BINDING_PROFILE_1;
        static const BindingProfile DEFAULT_GAMEPAD_BINDING_PROFILE_2;

        // Fields

        std::unordered_map<BindingProfileId, BindingProfile> _bindings  = {}; // Key = binding profile ID, value = binding profile.
        std::unordered_map<ActionId, bool>                   _conflicts = {}; // Key = action ID, value = has conflict.

    public:
        // Constructors

        BindingManager() = default;

        // Getters
        
        EventId               GetBoundEventId(BindingProfileId profileId, ActionId actionId);
        const BindingProfile& GetBindingProfile(BindingProfileId profileId);

        // Setters

        void SetEventBinding(BindingProfileId profileId, ActionId actionId, EventId eventId);
        void SetBindingProfile(BindingProfileId profileId, const BindingProfile& profile);
        void SetConflict(ActionId actionId, bool val);

        // Inquirers

        bool TestConflict(ActionId actionId) const;

        // Utilities

        void Initialize();
    };
}
