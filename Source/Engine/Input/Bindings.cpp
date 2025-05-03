#include "Framework.h"
#include "Engine/Input/Bindings.h"

#include "Engine/Input/Action.h"
#include "Engine/Input/Events.h"

namespace Silent::Input
{
    const BindingProfile BindingManager::DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE_0 =
    {
        { In::Up,        EventId::Up },
        { In::Down,      EventId::Down },
        { In::Left,      EventId::Left },
        { In::Right,     EventId::Right },

        { In::Action,    EventId::Ctrl },
        { In::Aim,       EventId::Space },
        { In::Light,     EventId::L },
        { In::Run,       EventId::Z },
        { In::View,      EventId::Pad0 },
        { In::StepLeft,  EventId::A },
        { In::StepRight, EventId::D },
        { In::Pause,     EventId::P },
        { In::Item,      EventId::I },
        { In::Map,       EventId::M },
        { In::Option,    EventId::O },
        
        { In::Enter,     EventId::Return },
        { In::Cancel,    EventId::Escape },
        { In::Skip,      EventId::Escape }
    };

    const BindingProfile BindingManager::DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE_1 =
    {
        { In::Up,        EventId::W },
        { In::Down,      EventId::S },
        { In::Left,      EventId::A },
        { In::Right,     EventId::D },

        { In::Action,    EventId::Ctrl },
        { In::Aim,       EventId::Space },
        { In::Light,     EventId::L },
        { In::Run,       EventId::J },
        { In::View,      EventId::K },
        { In::StepLeft,  EventId::Q },
        { In::StepRight, EventId::E },
        { In::Pause,     EventId::P },
        { In::Item,      EventId::I },
        { In::Map,       EventId::M },
        { In::Option,    EventId::O },
        
        { In::Enter,     EventId::Return },
        { In::Cancel,    EventId::Escape },
        { In::Skip,      EventId::Escape }
    };

    const BindingProfile BindingManager::DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE_2 =
    {
        { In::Up,        EventId::W },
        { In::Down,      EventId::S },
        { In::Left,      EventId::A },
        { In::Right,     EventId::D },

        { In::Action,    EventId::ClickLeft }, // F
        { In::Aim,       EventId::ClickRight },
        { In::Light,     EventId::L },
        { In::Run,       EventId::Shift },
        { In::View,      EventId::Ctrl },
        { In::StepLeft,  EventId::Q },
        { In::StepRight, EventId::E },
        { In::Pause,     EventId::P },
        { In::Item,      EventId::I },
        { In::Map,       EventId::M },
        { In::Option,    EventId::O },
        
        { In::Enter,     EventId::Return },
        { In::Cancel,    EventId::Escape },
        { In::Skip,      EventId::Escape }
    };

    EventId BindingManager::GetBoundEventId(BindingProfileId profileId, ActionId actionId)
    {
        // Find binding profile.
        auto bindingProfileIt = _bindings.find(profileId);
        if (bindingProfileIt == _bindings.end())
        {
            EventId::None;
        }

        // Get binding profile.
        const auto& [keyConfigType, bindingProfile] = *bindingProfileIt;

        // Find event-action binding.
        auto eventIt = bindingProfile.find(actionId);
        if (eventIt == bindingProfile.end())
        {
            EventId::None;
        }

        // Return bound event ID.
        auto [keyActionId, eventId] = *eventIt;
        return eventId;
    }

    const BindingProfile& BindingManager::GetBindingProfile(BindingProfileId profileId)
    {
        // Find binding profile.
        auto bindingProfileIt = _bindings.find(profileId);
        Assert(bindingProfileIt != _bindings.end(), "Attempted to get missing binding profile " + std::to_string((int)profileId) + ".");

        // Return binding profile.
        const auto& [keyConfigType, bindingProfile] = *bindingProfileIt;
        return bindingProfile;
    }

    void BindingManager::SetEventBinding(BindingProfileId profileId, ActionId actionId, EventId eventId)
    {
        // Overwrite or add new event-action binding.
        _bindings[profileId][actionId] = eventId;
    }

    void BindingManager::SetBindingProfile(BindingProfileId profileId, const BindingProfile& profile)
    {
        // Overwrite or add new binding profile.
        _bindings[profileId] = profile;
    }

    void BindingManager::SetConflict(ActionId actionId, bool val)
    {
        _conflicts[actionId] = val;
    }

    bool BindingManager::TestConflict(ActionId actionId) const
    {
        return _conflicts.at(actionId);
    }
    
    void BindingManager::Initialize()
    {
        // Initialize default bindings.
        _bindings =
        {
            { BindingProfileId::KeyboardMouseDefault0, DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE_0 },
            { BindingProfileId::KeyboardMouseDefault1, DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE_1 },
            { BindingProfileId::KeyboardMouseDefault2, DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE_2 },
            { BindingProfileId::KeyboardMouseCustom,   DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE_0 },
            { BindingProfileId::GamepadDefault0,       DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE_0 },
            { BindingProfileId::GamepadDefault1,       DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE_1 },
            { BindingProfileId::GamepadDefault2,       DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE_2 },
            { BindingProfileId::GamepadCustom,         DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE_0 }
        };

        // Initialize conflicts.
        _conflicts.reserve((int)ActionId::Count);
        for (int i = 0; i < (int)ActionId::Count; i++)
        {
            auto actionId = (ActionId)i;
            _conflicts.insert({ actionId, false });
        }
    }
}
