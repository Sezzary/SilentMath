#include "Framework.h"
#include "Engine/Input/Bindings.h"

#include "Engine/Input/Action.h"
#include "Engine/Input/Events.h"
#include "Engine/Time.h"

namespace Silent::Input
{
    const BindingProfile DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE_0 =
    {
        { In::Up,        { EventId::W, EventId::Up } },
        { In::Down,      { EventId::S, EventId::Down } },
        { In::Left,      { EventId::A, EventId::Left } },
        { In::Right,     { EventId::D, EventId::Right } },

        { In::Enter,     { EventId::Return } },
        { In::Cancel,    { EventId::Escape } },
        { In::Skip,      { EventId::Escape } },

        { In::Action,    { EventId::Ctrl } },
        { In::Aim,       { EventId::Space } },
        { In::Light,     { EventId::L } },
        { In::Run,       { EventId::J } },
        { In::View,      { EventId::K } },
        { In::StepLeft,  { EventId::Q } },
        { In::StepRight, { EventId::E } },
        { In::Pause,     { EventId::P } },
        { In::Item,      { EventId::I } },
        { In::Map,       { EventId::M } },
        { In::Option,    { EventId::O } }
    };

    const BindingProfile DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE_1 =
    {
        { In::Up,        { EventId::W, EventId::Up } },
        { In::Down,      { EventId::S, EventId::Down } },
        { In::Left,      { EventId::A, EventId::Left } },
        { In::Right,     { EventId::D, EventId::Right } },

        { In::Enter,     { EventId::Return } },
        { In::Cancel,    { EventId::Escape } },
        { In::Skip,      { EventId::Escape } },

        { In::Action,    { EventId::ClickLeft, EventId::F } },
        { In::Aim,       { EventId::ClickRight } },
        { In::Light,     { EventId::L } },
        { In::Run,       { EventId::Shift } },
        { In::View,      { EventId::Ctrl } },
        { In::StepLeft,  { EventId::Q } },
        { In::StepRight, { EventId::E } },
        { In::Pause,     { EventId::P } },
        { In::Item,      { EventId::I } },
        { In::Map,       { EventId::M } },
        { In::Option,    { EventId::O } }
    };

    const BindingProfile DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE_2 =
    {
        { In::Up,        { EventId::Up } },
        { In::Down,      { EventId::Down } },
        { In::Left,      { EventId::Left } },
        { In::Right,     { EventId::Right } },

        { In::Enter,     { EventId::Return } },
        { In::Cancel,    { EventId::Escape } },
        { In::Skip,      { EventId::Escape } },

        { In::Action,    { EventId::Ctrl } },
        { In::Aim,       { EventId::Space } },
        { In::Light,     { EventId::L } },
        { In::Run,       { EventId::Z } },
        { In::View,      { EventId::Pad0 } },
        { In::StepLeft,  { EventId::A } },
        { In::StepRight, { EventId::D } },
        { In::Pause,     { EventId::P } },
        { In::Item,      { EventId::I } },
        { In::Map,       { EventId::M } },
        { In::Option,    { EventId::O } }
    };

    const BindingProfile DEFAULT_GAMEPAD_BINDING_PROFILE_0 =
    {
        { In::Up,        { EventId::DpadUp, EventId::StickLeftYNeg } },
        { In::Down,      { EventId::DpadDown, EventId::StickLeftYPos } },
        { In::Left,      { EventId::DpadLeft, EventId::StickLeftXNeg } },
        { In::Right,     { EventId::DpadRight, EventId::StickLeftXPos } },

        { In::Enter,     { EventId::South, EventId::Start } },
        { In::Cancel,    { EventId::North, EventId::East, EventId::West } },
        { In::Skip,      { EventId::Start } },

        { In::Action,    { EventId::South } },
        { In::Aim,       { EventId::TriggerRight } },
        { In::Light,     { EventId::East } },
        { In::Run,       { EventId::West } },
        { In::View,      { EventId::TriggerLeft } },
        { In::StepLeft,  { EventId::ShoulderLeft } },
        { In::StepRight, { EventId::ShoulderRight} },
        { In::Pause,     { EventId::Start } },
        { In::Item,      { EventId::Select } },
        { In::Map,       { EventId::North } },
        { In::Option,    { EventId::O } }
    };

    const BindingProfile DEFAULT_GAMEPAD_BINDING_PROFILE_1 =
    {
        { In::Up,        { EventId::DpadUp, EventId::StickLeftYNeg } },
        { In::Down,      { EventId::DpadDown, EventId::StickLeftYPos } },
        { In::Left,      { EventId::DpadLeft, EventId::StickLeftXNeg } },
        { In::Right,     { EventId::DpadRight, EventId::StickLeftXPos } },
        
        { In::Enter,     { EventId::South, EventId::Start } },
        { In::Cancel,    { EventId::North, EventId::West, EventId::East } },
        { In::Skip,      { EventId::Start } },

        { In::Action,    { EventId::South } },
        { In::Aim,       { EventId::ShoulderRight } },
        { In::Light,     { EventId::East } },
        { In::Run,       { EventId::West } },
        { In::View,      { EventId::ShoulderLeft } },
        { In::StepLeft,  { EventId::TriggerLeft } },
        { In::StepRight, { EventId::TriggerRight} },
        { In::Pause,     { EventId::Start } },
        { In::Item,      { EventId::Select } },
        { In::Map,       { EventId::North } },
        { In::Option,    { EventId::O } }
    };

    const BindingProfile DEFAULT_GAMEPAD_BINDING_PROFILE_2 =
    {
        { In::Up,        { EventId::DpadUp, EventId::StickLeftYNeg } },
        { In::Down,      { EventId::DpadDown, EventId::StickLeftYPos } },
        { In::Left,      { EventId::DpadLeft, EventId::StickLeftXNeg } },
        { In::Right,     { EventId::DpadRight, EventId::StickLeftXPos } },
        
        { In::Enter,     { EventId::South, EventId::Start } },
        { In::Cancel,    { EventId::North, EventId::West, EventId::East } },
        { In::Skip,      { EventId::Start } },

        { In::Action,    { EventId::South } },
        { In::Aim,       { EventId::TriggerRight } },
        { In::Light,     { EventId::East } },
        { In::Run,       { EventId::West } },
        { In::View,      { EventId::TriggerLeft } },
        { In::StepLeft,  { EventId::ShoulderLeft } },
        { In::StepRight, { EventId::ShoulderRight} },
        { In::Pause,     { EventId::Start } },
        { In::Item,      { EventId::North } },
        { In::Map,       { EventId::Select } },
        { In::Option,    { EventId::O } }
    };

    const std::vector<BindingProfileId> KEYBOARD_MOUSE_BINDING_PROFILE_IDS
    {
        BindingProfileId::KeyboardMouseDefault0,
        BindingProfileId::KeyboardMouseDefault1,
        BindingProfileId::KeyboardMouseDefault2,
        BindingProfileId::KeyboardMouseCustom
    };

    const std::vector<BindingProfileId> GAMEPAD_BINDING_PROFILE_IDS
    {
        BindingProfileId::GamepadDefault0,
        BindingProfileId::GamepadDefault1,
        BindingProfileId::GamepadDefault2,
        BindingProfileId::GamepadCustom
    };

    std::vector<EventId> BindingManager::GetBoundEventIds(BindingProfileId profileId, ActionId actionId)
    {
        // Find binding profile.
        auto profileIt = _bindings.find(profileId);
        if (profileIt == _bindings.end())
        {
            return {};
        }

        // Get binding profile.
        const auto& [keyConfigType, profile] = *profileIt;

        // Find event-action binding.
        auto eventIt = profile.find(actionId);
        if (eventIt == profile.end())
        {
            return {};
        }

        // Return bound event IDs.
        auto [keyActionId, eventIds] = *eventIt;
        return eventIds;
    }

    const BindingProfile& BindingManager::GetBindingProfile(BindingProfileId profileId)
    {
        // Find binding profile.
        auto profileIt = _bindings.find(profileId);
        Assert(profileIt != _bindings.end(), "Attempted to get missing binding profile " + std::to_string((int)profileId) + ".");

        // Return binding profile.
        const auto& [keyConfigType, profile] = *profileIt;
        return profile;
    }

    void BindingManager::SetEventBinding(BindingProfileId profileId, ActionId actionId, EventId eventId)
    {
        // Overwrite or add new event-action binding. For simplicity, can only set bind single event IDs.
        _bindings[profileId][actionId] = { eventId };
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
        // Initialize bindings.
        _bindings =
        {
            { BindingProfileId::KeyboardMouseDefault0, DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE_0 },
            { BindingProfileId::KeyboardMouseDefault1, DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE_1 },
            { BindingProfileId::KeyboardMouseDefault2, DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE_2 },
            { BindingProfileId::KeyboardMouseCustom,   DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE_0 },
            { BindingProfileId::GamepadDefault0,       DEFAULT_GAMEPAD_BINDING_PROFILE_0 },
            { BindingProfileId::GamepadDefault1,       DEFAULT_GAMEPAD_BINDING_PROFILE_1 },
            { BindingProfileId::GamepadDefault2,       DEFAULT_GAMEPAD_BINDING_PROFILE_2 },
            { BindingProfileId::GamepadCustom,         DEFAULT_GAMEPAD_BINDING_PROFILE_0 }
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
