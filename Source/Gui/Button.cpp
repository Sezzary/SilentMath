#include "Framework.h"
#include "Gui/Button.h"

#include "Application.h"
#include "Input/Input.h"
#include "Renderer/Common/Enums.h"
#include "Utils/Utils.h"

using namespace Silent::Input;
using namespace Silent::Renderer;
using namespace Silent::Utils;

namespace Silent::Gui
{
    Button::Button(const Rectangle& bounds, ScaleMode scaleMode,
                   const std::optional<Callback>& onEnter,
                   const std::optional<Callback>& onInside,
                   const std::optional<Callback>& onLeave,
                   const std::optional<Callback>& onOutside,
                   const std::optional<Callback>& onClick,
                   const std::optional<Callback>& onHold,
                   const std::optional<Callback>& onRelease)
    {
        _bounds    = bounds;
        _scaleMode = scaleMode;
        _isActive  = false;

        _onEnter   = onEnter;
        _onInside  = onInside;
        _onLeave   = onLeave;
        _onOutside = onOutside;
        _onClick   = onClick;
        _onHold    = onHold;
        _onRelease = onRelease;
    }

    void Button::Update(bool isFocused)
    {
        constexpr auto SELECT_ACTION_IDS = std::array<ActionId, 2>
        {
            In::Enter,
            In::Action
        };

        Update(isFocused, ToSpan(SELECT_ACTION_IDS));
    }

    void Button::Update(const Vector2& point)
    {
        constexpr auto SELECT_ACTION_IDS = std::array<ActionId, 1>
        {
            In::MouseClickLeft
        };

        Update(_bounds.Intersects(point), ToSpan(SELECT_ACTION_IDS));
    }

    bool Button::CheckClickedAction(const std::span<const ActionId>& actionIds) const
    {
        const auto& input = g_App.GetInput();

        for (auto actionId : actionIds)
        {
            if (input.GetAction(actionId).IsClicked())
            {
                return true;
            }
        }

        return false;
    }

    bool Button::CheckHeldAction(const std::span<const ActionId>& actionIds) const
    {
        const auto& input = g_App.GetInput();

        for (auto actionId : actionIds)
        {
            if (input.GetAction(actionId).IsHeld())
            {
                return true;
            }
        }

        return false;
    }

    bool Button::CheckReleasedAction(const std::span<const ActionId>& actionIds) const
    {
        const auto& input = g_App.GetInput();

        for (auto actionId : actionIds)
        {
            if (input.GetAction(actionId).IsReleased())
            {
                return true;
            }
        }

        return false;
    }

    void Button::Update(bool isActive, const std::span<const ActionId>& selectActionIds)
    {
        if (isActive)
        {
            // Execute On Enter callback if previous active state was outside.
            if (!_isActive)
            {
                ExecuteCallback(_onEnter);
                _isActive = true;
            }
            // Execute On Inside callback if previous active state was also inside.
            else
            {
                ExecuteCallback(_onInside);
            }

            // Execute On Click callback if select action is clicked.
            if (CheckClickedAction(selectActionIds))
            {
                ExecuteCallback(_onClick);
            }
            // Execute On Hold callback if select action is held.
            else if (CheckHeldAction(selectActionIds))
            {
                ExecuteCallback(_onHold);
            }
            // Execute On Release callback if select action is released.
            else if (CheckReleasedAction(selectActionIds))
            {
                ExecuteCallback(_onRelease);
            }
        }
        else
        {
            // Execute On Leave callback if previous active state was inside.
            if (_isActive)
            {
                ExecuteCallback(_onLeave);
                _isActive = false;
            }
            // Execute On Outside callback if previous active state was also outside.
            else
            {
                ExecuteCallback(_onOutside);
            }
        }
    }

    void Button::ExecuteCallback(const std::optional<Callback>& callback) const
    {
        if (callback.has_value())
        {
            (*callback)();
        }
    }
}
