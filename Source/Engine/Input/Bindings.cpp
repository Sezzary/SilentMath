#include "Framework.h"
#include "Engine/Input/Bindings.h"

#include "Engine/Input/Action.h"
#include "Engine/Input/Events.h"

namespace Silent::Input
{
    const BindingProfile BindingManager::DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE =
    {
        { In::Up,        EventId::W },
        { In::Down,      EventId::S },
        { In::Left,      EventId::A },
        { In::Right,     EventId::D },
        { In::StepLeft,  EventId::Q },
        { In::StepRight, EventId::E },
        { In::Action,    EventId::Ctrl },
        { In::Aim,       EventId::Space },
        { In::Run,       EventId::K },
        { In::View,      EventId::L },
        { In::Light,     EventId::N },
        { In::Map,       EventId::M },
        { In::Skip,      EventId::U },
        { In::Item,      EventId::I },
        { In::Option,    EventId::O },
        { In::Pause,     EventId::P },
        { In::Enter,     EventId::Return },
        { In::Cancel,    EventId::Escape }
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
    
    void BindingProfileId::Initialize()
    {
        // Initialize default bindings.
        _bindings.reserve((int)BindingProfileId::Count);
        _bindings =
        {
            { BindingProfileId::KeyboardMouseDefault, DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE },
            { BindingProfileId::KeyboardMouseCustom,  DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE },
            { BindingProfileId::GamepadDefault,       DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE },
            { BindingProfileId::GamepadCustom,        DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE }
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
