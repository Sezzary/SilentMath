#include "Framework.h"
#include "Input/Binding.h"

#include "Input/Action.h"
#include "Input/Event.h"
#include "Services/Clock.h"
#include "Utils/Utils.h"

using namespace Silent::Services;
using namespace Silent::Utils;

namespace Silent::Input
{
    static const auto DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE_TYPE_1 = BindingProfile
    {
        { In::Up,        { EventId::W, EventId::Up                   } },
        { In::Down,      { EventId::S, EventId::Down                 } },
        { In::Left,      { EventId::A, EventId::Left                 } },
        { In::Right,     { EventId::D, EventId::Right                } },

        { In::Enter,     { EventId::Return                           } },
        { In::Cancel,    { EventId::Escape                           } },

        { In::Action,    { EventId::MouseClickLeft,   EventId::Ctrl  } },
        { In::Aim,       { EventId::MouseClickRight,  EventId::Space } },
        { In::Light,     { EventId::MouseClickMiddle, EventId::L     } },
        { In::Run,       { EventId::Shift,                           } },
        { In::View,      { EventId::F                                } },
        { In::StepLeft,  { EventId::Q, EventId::Delete,              } },
        { In::StepRight, { EventId::E, EventId::PageDown             } },
        { In::Pause,     { EventId::P                                } },
        { In::Item,      { EventId::Escape, EventId::I               } },
        { In::Map,       { EventId::M                                } },
        { In::Option,    { EventId::O                                } }
    };

    static const auto DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE_TYPE_2 = BindingProfile
    {
        { In::Up,        { EventId::W                  } },
        { In::Down,      { EventId::S                  } },
        { In::Left,      { EventId::A                  } },
        { In::Right,     { EventId::D                  } },

        { In::Enter,     { EventId::Return             } },
        { In::Cancel,    { EventId::Escape             } },

        { In::Action,    { EventId::Ctrl               } },
        { In::Aim,       { EventId::Space              } },
        { In::Light,     { EventId::L                  } },
        { In::Run,       { EventId::J                  } },
        { In::View,      { EventId::K                  } },
        { In::StepLeft,  { EventId::Q                  } },
        { In::StepRight, { EventId::E                  } },
        { In::Pause,     { EventId::P                  } },
        { In::Item,      { EventId::Escape, EventId::I } },
        { In::Map,       { EventId::M                  } },
        { In::Option,    { EventId::O                  } }
    };

    static const auto DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE_TYPE_3 = BindingProfile
    {
        { In::Up,        { EventId::Up                   } },
        { In::Down,      { EventId::Down                 } },
        { In::Left,      { EventId::Left                 } },
        { In::Right,     { EventId::Right                } },

        { In::Enter,     { EventId::Return               } },
        { In::Cancel,    { EventId::Escape               } },

        { In::Action,    { EventId::Ctrl,                } },
        { In::Aim,       { EventId::Space                } },
        { In::Light,     { EventId::L                    } },
        { In::Run,       { EventId::Shift                } },
        { In::View,      { EventId::Pad0                 } },
        { In::StepLeft,  { EventId::Q, EventId::Delete,  } },
        { In::StepRight, { EventId::E, EventId::PageDown } },
        { In::Pause,     { EventId::P                    } },
        { In::Item,      { EventId::Escape, EventId::I   } },
        { In::Map,       { EventId::M                    } },
        { In::Option,    { EventId::O                    } }
    };

    static const auto DEFAULT_GAMEPAD_BINDING_PROFILE_TYPE_1 = BindingProfile
    {
        { In::Up,        { EventId::GamepadDpadUp,    EventId::GamepadStickLeftUp            } },
        { In::Down,      { EventId::GamepadDpadDown,  EventId::GamepadStickLeftDown          } },
        { In::Left,      { EventId::GamepadDpadLeft,  EventId::GamepadStickLeftLeft          } },
        { In::Right,     { EventId::GamepadDpadRight, EventId::GamepadStickLeftRight         } },

        { In::Enter,     { EventId::GamepadSouth, EventId::GamepadStart                      } },
        { In::Cancel,    { EventId::GamepadNorth, EventId::GamepadEast, EventId::GamepadWest } },

        { In::Action,    { EventId::GamepadSouth                                             } },
        { In::Aim,       { EventId::GamepadTriggerRight                                      } },
        { In::Light,     { EventId::GamepadEast                                              } },
        { In::Run,       { EventId::GamepadWest                                              } },
        { In::View,      { EventId::GamepadTriggerLeft                                       } },
        { In::StepLeft,  { EventId::GamepadShoulderLeft                                      } },
        { In::StepRight, { EventId::GamepadShoulderRight                                     } },
        { In::Pause,     { EventId::GamepadStart                                             } },
        { In::Item,      { EventId::GamepadSelect                                            } },
        { In::Map,       { EventId::GamepadNorth                                             } },
        { In::Option,    { EventId::O                                                        } }
    };

    static const auto DEFAULT_GAMEPAD_BINDING_PROFILE_TYPE_2 = BindingProfile
    {
        { In::Up,        { EventId::GamepadDpadUp,    EventId::GamepadStickLeftUp            } },
        { In::Down,      { EventId::GamepadDpadDown,  EventId::GamepadStickLeftDown          } },
        { In::Left,      { EventId::GamepadDpadLeft,  EventId::GamepadStickLeftLeft          } },
        { In::Right,     { EventId::GamepadDpadRight, EventId::GamepadStickLeftRight         } },
        
        { In::Enter,     { EventId::GamepadSouth, EventId::GamepadStart                      } },
        { In::Cancel,    { EventId::GamepadNorth, EventId::GamepadWest, EventId::GamepadEast } },

        { In::Action,    { EventId::GamepadSouth                                             } },
        { In::Aim,       { EventId::GamepadShoulderRight                                     } },
        { In::Light,     { EventId::GamepadEast                                              } },
        { In::Run,       { EventId::GamepadWest                                              } },
        { In::View,      { EventId::GamepadShoulderLeft                                      } },
        { In::StepLeft,  { EventId::GamepadTriggerLeft                                       } },
        { In::StepRight, { EventId::GamepadTriggerRight                                      } },
        { In::Pause,     { EventId::GamepadStart                                             } },
        { In::Item,      { EventId::GamepadSelect                                            } },
        { In::Map,       { EventId::GamepadNorth                                             } },
        { In::Option,    { EventId::O                                                        } }
    };

    static const auto DEFAULT_GAMEPAD_BINDING_PROFILE_TYPE_3 = BindingProfile
    {
        { In::Up,        { EventId::GamepadDpadUp,    EventId::GamepadStickLeftUp            } },
        { In::Down,      { EventId::GamepadDpadDown,  EventId::GamepadStickLeftDown          } },
        { In::Left,      { EventId::GamepadDpadLeft,  EventId::GamepadStickLeftLeft          } },
        { In::Right,     { EventId::GamepadDpadRight, EventId::GamepadStickLeftRight         } },
        
        { In::Enter,     { EventId::GamepadSouth, EventId::GamepadStart                      } },
        { In::Cancel,    { EventId::GamepadNorth, EventId::GamepadWest, EventId::GamepadEast } },

        { In::Action,    { EventId::GamepadSouth                                             } },
        { In::Aim,       { EventId::GamepadTriggerRight                                      } },
        { In::Light,     { EventId::GamepadEast                                              } },
        { In::Run,       { EventId::GamepadWest                                              } },
        { In::View,      { EventId::GamepadTriggerLeft                                       } },
        { In::StepLeft,  { EventId::GamepadShoulderLeft                                      } },
        { In::StepRight, { EventId::GamepadShoulderRight                                     } },
        { In::Pause,     { EventId::GamepadStart                                             } },
        { In::Item,      { EventId::GamepadNorth                                             } },
        { In::Map,       { EventId::GamepadSelect                                            } },
        { In::Option,    { EventId::O                                                        } }
    };

    static const auto RAW_KEYBOARD_BINDING_PROFILE = BindingProfile
    {
        { In::A,            { EventId::A                          } },
        { In::B,            { EventId::B                          } },
        { In::C,            { EventId::C                          } },
        { In::D,            { EventId::D                          } },
        { In::E,            { EventId::E                          } },
        { In::F,            { EventId::F                          } },
        { In::G,            { EventId::G                          } },
        { In::H,            { EventId::H                          } },
        { In::I,            { EventId::I                          } },
        { In::J,            { EventId::J                          } },
        { In::K,            { EventId::K                          } },
        { In::L,            { EventId::L                          } },
        { In::M,            { EventId::M                          } },
        { In::N,            { EventId::N                          } },
        { In::O,            { EventId::O                          } },
        { In::P,            { EventId::P                          } },
        { In::Q,            { EventId::Q                          } },
        { In::R,            { EventId::R                          } },
        { In::S,            { EventId::S                          } },
        { In::T,            { EventId::T                          } },
        { In::U,            { EventId::U                          } },
        { In::V,            { EventId::V                          } },
        { In::W,            { EventId::W                          } },
        { In::X,            { EventId::X                          } },
        { In::Y,            { EventId::Y                          } },
        { In::Z,            { EventId::Z                          } },
        { In::Num1,         { EventId::Row1,   EventId::Pad1      } },
        { In::Num2,         { EventId::Row2,   EventId::Pad2      } },
        { In::Num3,         { EventId::Row3,   EventId::Pad3      } },
        { In::Num4,         { EventId::Row4,   EventId::Pad4      } },
        { In::Num5,         { EventId::Row5,   EventId::Pad5      } },
        { In::Num6,         { EventId::Row6,   EventId::Pad6      } },
        { In::Num7,         { EventId::Row7,   EventId::Pad7      } },
        { In::Num8,         { EventId::Row8,   EventId::Pad8      } },
        { In::Num9,         { EventId::Row9,   EventId::Pad9      } },
        { In::Num0,         { EventId::Row0,   EventId::Pad0      } },
        { In::Return,       { EventId::Return, EventId::PadEnter  } },
        { In::Escape,       { EventId::Escape                     } },
        { In::Backspace,    { EventId::Backspace                  } },
        { In::Tab,          { EventId::Tab                        } },
        { In::Space,        { EventId::Space                      } },
        { In::Home,         { EventId::Home                       } },
        { In::End,          { EventId::End                        } },
        { In::Delete,       { EventId::Delete                     } },
        { In::Minus,        { EventId::Minus,  EventId::PadMinus  } },
        { In::Equals,       { EventId::Equals, EventId::PadPlus   } },
        { In::BracketLeft,  { EventId::BracketLeft                } },
        { In::BracketRight, { EventId::BracketRight               } },
        { In::Backslash,    { EventId::Backslash                  } },
        { In::Semicolon,    { EventId::Semicolon                  } },
        { In::Apostrophe,   { EventId::Apostrophe                 } },
        { In::Comma,        { EventId::Comma                      } },
        { In::Period,       { EventId::Period, EventId::PadPeriod } },
        { In::Slash,        { EventId::Slash,  EventId::PadDivide } },
        { In::ArrowUp,      { EventId::Up                         } },
        { In::ArrowDown,    { EventId::Down                       } },
        { In::ArrowLeft,    { EventId::Left                       } },
        { In::ArrowRight,   { EventId::Right                      } },
        { In::Ctrl,         { EventId::Ctrl                       } },
        { In::Shift,        { EventId::Shift                      } },
        { In::Alt,          { EventId::Alt                        } }
    };

    static const auto RAW_MOUSE_BINDING_PROFILE = BindingProfile
    {
        { In::MouseClickLeft,   { EventId::MouseClickLeft   } },
        { In::MouseClickMiddle, { EventId::MouseClickMiddle } },
        { In::MouseClickRight,  { EventId::MouseClickRight  } },
        { In::MouseScrollUp,    { EventId::MouseScrollUp    } },
        { In::MouseScrollDown,  { EventId::MouseScrollDown  } },
        { In::MouseUp,          { EventId::MouseUp          } },
        { In::MouseDown,        { EventId::MouseDown        } },
        { In::MouseLeft,        { EventId::MouseLeft        } },
        { In::MouseRight,       { EventId::MouseRight       } }
    };

    static const auto RAW_GAMEPAD_BINDING_PROFILE = BindingProfile
    {
        { In::GamepadNorth,           { EventId::GamepadNorth           } },
        { In::GamepadSouth,           { EventId::GamepadSouth           } },
        { In::GamepadEast,            { EventId::GamepadEast            } },
        { In::GamepadWest,            { EventId::GamepadWest            } },
        { In::GamepadStart,           { EventId::GamepadStart           } },
        { In::GamepadSelect,          { EventId::GamepadSelect          } },
        { In::GamepadShoulderLeft,    { EventId::GamepadShoulderLeft    } },
        { In::GamepadShoulderRight,   { EventId::GamepadShoulderRight   } },
        { In::GamepadTriggerLeft,     { EventId::GamepadTriggerLeft     } },
        { In::GamepadTriggerRight,    { EventId::GamepadTriggerRight    } },
        { In::GamepadDpadUp,          { EventId::GamepadDpadUp          } },
        { In::GamepadDpadDown,        { EventId::GamepadDpadDown        } },
        { In::GamepadDpadLeft,        { EventId::GamepadDpadLeft        } },
        { In::GamepadDpadRight,       { EventId::GamepadDpadRight       } },
        { In::GamepadStickLeftIn,     { EventId::GamepadStickLeft       } },
        { In::GamepadStickLeftUp,     { EventId::GamepadStickLeftUp     } },
        { In::GamepadStickLeftDown,   { EventId::GamepadStickLeftDown   } },
        { In::GamepadStickLeftLeft,   { EventId::GamepadStickLeftLeft   } },
        { In::GamepadStickLeftRight,  { EventId::GamepadStickLeftRight  } },
        { In::GamepadStickRightIn,    { EventId::GamepadStickRight      } },
        { In::GamepadStickRightUp,    { EventId::GamepadStickRightUp    } },
        { In::GamepadStickRightDown,  { EventId::GamepadStickRightDown  } },
        { In::GamepadStickRightLeft,  { EventId::GamepadStickRightLeft  } },
        { In::GamepadStickRightRight, { EventId::GamepadStickRightRight } }
    };

    const BindingProfile DEFAULT_CUSTOM_KEYBOARD_MOUSE_BINDING_PROFILE =
    {
        { In::Up,        { EventId::W                } },
        { In::Down,      { EventId::S                } },
        { In::Left,      { EventId::A                } },
        { In::Right,     { EventId::D                } },

        { In::Enter,     { EventId::Return           } },
        { In::Cancel,    { EventId::Escape           } },

        { In::Action,    { EventId::MouseClickLeft   } },
        { In::Aim,       { EventId::MouseClickRight  } },
        { In::Light,     { EventId::MouseClickMiddle } },
        { In::Run,       { EventId::Shift,           } },
        { In::View,      { EventId::F                } },
        { In::StepLeft,  { EventId::Q                } },
        { In::StepRight, { EventId::E                } },
        { In::Pause,     { EventId::P                } },
        { In::Item,      { EventId::Escape           } },
        { In::Map,       { EventId::M                } },
        { In::Option,    { EventId::O                } }
    };

    const BindingProfile DEFAULT_CUSTOM_GAMEPAD_BINDING_PROFILE =
    {
        { In::Up,        { EventId::GamepadDpadUp        } },
        { In::Down,      { EventId::GamepadDpadDown      } },
        { In::Left,      { EventId::GamepadDpadLeft      } },
        { In::Right,     { EventId::GamepadDpadRight     } },

        { In::Enter,     { EventId::GamepadSouth         } },
        { In::Cancel,    { EventId::GamepadNorth         } },

        { In::Action,    { EventId::GamepadSouth         } },
        { In::Aim,       { EventId::GamepadTriggerRight  } },
        { In::Light,     { EventId::GamepadEast          } },
        { In::Run,       { EventId::GamepadWest          } },
        { In::View,      { EventId::GamepadTriggerLeft   } },
        { In::StepLeft,  { EventId::GamepadShoulderLeft  } },
        { In::StepRight, { EventId::GamepadShoulderRight } },
        { In::Pause,     { EventId::GamepadStart         } },
        { In::Item,      { EventId::GamepadSelect        } },
        { In::Map,       { EventId::GamepadNorth         } },
        { In::Option,    { EventId::O                    } }
    };

    const std::vector<BindingProfileId> DEFAULT_BINDING_PROFILE_IDS =
    {
        BindingProfileId::DefaultKeyboardMouseType1,
        BindingProfileId::DefaultKeyboardMouseType2,
        BindingProfileId::DefaultKeyboardMouseType3,
        BindingProfileId::DefaultGamepadType1,
        BindingProfileId::DefaultGamepadType2,
        BindingProfileId::DefaultGamepadType3
    };

    const std::vector<BindingProfileId> CUSTOM_BINDING_PROFILE_IDS =
    {
        BindingProfileId::CustomKeyboardMouse,
        BindingProfileId::CustomGamepad
    };

    const std::vector<BindingProfileId> RAW_BINDING_PROFILE_IDS =
    {
        BindingProfileId::RawKeyboard,
        BindingProfileId::RawMouse,
        BindingProfileId::RawGamepad
    };

    const BindingProfile& BindingManager::GetProfile(BindingProfileId profileId) const
    {
        return _bindings[(int)profileId];
    }

    void BindingManager::SetProfile(BindingProfileId profileId, const BindingProfile& newProfile)
    {
        // Check if profile is customizable.
        if (!Contains(CUSTOM_BINDING_PROFILE_IDS, profileId))
        {
            Debug::Log(Fmt("Attempted to set all bindings for non-customizable binding profile {}.", (int)profileId),
                       Debug::LogLevel::Warning);
            return;
        }

        // Set new bindings.
        _bindings[(int)profileId] = newProfile;
    }

    void BindingManager::SetBinding(BindingProfileId profileId, ActionId actionId, EventId eventId)
    {
        // Check if profile is customizable.
        if (!Contains(CUSTOM_BINDING_PROFILE_IDS, profileId))
        {
            Debug::Log(Fmt("Attempted to bind event {} to action {} for non-customizable binding profile {}.",
                       (int)eventId, (int)actionId, (int)profileId),
                       Debug::LogLevel::Warning);
            return;
        }

        auto& profile = _bindings[(int)profileId];

        // Swap binding if event is already bound to another action in same group.
        bool hasSwap = false;
        for (auto actionGroupId : USER_ACTION_GROUP_IDS)
        {
            // Check if action ID exists in current group.
            const auto& actionIds = ACTION_ID_GROUPS[(int)actionGroupId];
            if (!Contains(actionIds, actionId))
            {
                continue;
            }

            // Run through action IDs in group.
            for (auto otherActionId : actionIds)
            {
                // Skip action to bind.
                if (actionId == otherActionId)
                {
                    continue;
                }

                // Swap if event is already bound.
                auto& eventIds = profile.at(otherActionId);
                if (Contains(eventIds, eventId))
                {
                    std::swap(eventIds, profile[actionId]);
                    hasSwap = true;
                    break;
                }
            }

            break;
        }

        // Set new binding.
        if (!hasSwap)
        {
            profile[actionId] = { eventId };
        }
    }

    void BindingManager::Initialize(const BindingProfile& customKeyboardMouseProfile, const BindingProfile& customGamepadProfile)
    {
        _bindings =
        {
            /* `BindingProfileId::CustomKeyboardMouse`       */ customKeyboardMouseProfile,
            /* `BindingProfileId::CustomGamepad`             */ customGamepadProfile,
            /* `BindingProfileId::DefaultKeyboardMouseType1` */ DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE_TYPE_1,
            /* `BindingProfileId::DefaultKeyboardMouseType2` */ DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE_TYPE_2,
            /* `BindingProfileId::DefaultKeyboardMouseType3` */ DEFAULT_KEYBOARD_MOUSE_BINDING_PROFILE_TYPE_3,
            /* `BindingProfileId::DefaultGamepadType1`       */ DEFAULT_GAMEPAD_BINDING_PROFILE_TYPE_1,
            /* `BindingProfileId::DefaultGamepadType2`       */ DEFAULT_GAMEPAD_BINDING_PROFILE_TYPE_2,
            /* `BindingProfileId::DefaultGamepadType3`       */ DEFAULT_GAMEPAD_BINDING_PROFILE_TYPE_3,
            /* `BindingProfileId::RawKeyboard`               */ RAW_KEYBOARD_BINDING_PROFILE,
            /* `BindingProfileId::RawMouse`                  */ RAW_MOUSE_BINDING_PROFILE,
            /* `BindingProfileId::RawGamepad`                */ RAW_GAMEPAD_BINDING_PROFILE
        };
    }
}
