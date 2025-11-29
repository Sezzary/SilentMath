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
    Button::Button(const AxisAlignedBoundingRect& bounds, ScaleMode scaleMode,
                   const std::optional<Callback>& onEnter,
                   const std::optional<Callback>& onInside,
                   const std::optional<Callback>& onLeave,
                   const std::optional<Callback>& onOutside,
                   const std::optional<Callback>& onClick,
                   const std::optional<Callback>& onHold,
                   const std::optional<Callback>& onRelease)
    {
        _isActive = false;

        _bounds    = bounds;
        _scaleMode = scaleMode;

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
        static const auto SELECT_ACTION_IDS = std::vector<ActionId>
        {
            In::Enter,
            In::Action
        };

        Update(isFocused, SELECT_ACTION_IDS);
    }

    void Button::Update(const Vector2& point)
    {
        static const auto SELECT_ACTION_IDS = std::vector<ActionId>
        {
            In::MouseClickLeft
        };

        Update(_bounds.Intersects(point), SELECT_ACTION_IDS);
    }

    bool Button::CheckClickedAction(const std::vector<ActionId>& actionIds) const
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

    bool Button::CheckHeldAction(const std::vector<ActionId>& actionIds) const
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

    bool Button::CheckReleasedAction(const std::vector<ActionId>& actionIds) const
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

    void Button::Update(bool isActive, const std::vector<ActionId>& selectActionIds)
    {
        if (isActive)
        {
            // Execute `_onEnter` callback if previous state was inactive (outside).
            if (!_isActive)
            {
                ExecuteCallback(_onEnter);
            }
            // Execute `_onInside` callback if previous state was active (also inside).
            else
            {
                ExecuteCallback(_onInside);
            }

            // Execute `_onClick` and `_onHold` callbacks if select action is clicked.
            if (CheckClickedAction(selectActionIds))
            {
                ExecuteCallback(_onClick);
                ExecuteCallback(_onHold);
            }
            // Execute `_onHold` callback if select action is held.
            else if (CheckHeldAction(selectActionIds))
            {
                ExecuteCallback(_onHold);
            }
            // Execute `_onRelease` callback if select action is released.
            else if (CheckReleasedAction(selectActionIds))
            {
                ExecuteCallback(_onRelease);
            }

            _isActive = true;
        }
        else
        {
            // Execute `_onLeave` callback if previous state was active (inside).
            if (_isActive)
            {
                ExecuteCallback(_onLeave);
            }
            // Execute `_onOutside` callback if previous state was inactive (also outside).
            else
            {
                ExecuteCallback(_onOutside);
            }

            _isActive = false;
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
