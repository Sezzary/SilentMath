#include "Framework.h"
#include "Gui/Button.h"

#include "Application.h"
#include "Input/Input.h"
#include "Renderer/Common/Enums.h"
#include "Renderer/Common/Utils.h"
#include "Utils/Utils.h"

using namespace Silent::Input;
using namespace Silent::Renderer;
using namespace Silent::Utils;

namespace Silent::Gui
{
    Button::Button(const Vector2& center, const Vector2& extents, ScaleMode scaleMode,
                   const std::optional<Callback>& onEnter,
                   const std::optional<Callback>& onInside,
                   const std::optional<Callback>& onLeave,
                   const std::optional<Callback>& onOutside,
                   const std::optional<Callback>& onClick,
                   const std::optional<Callback>& onHold,
                   const std::optional<Callback>& onRelease)
    {
        _prevActiveState = false;

        _scaleMode = scaleMode;
        _onEnter   = onEnter;
        _onInside  = onInside;
        _onLeave   = onLeave;
        _onOutside = onOutside;
        _onClick   = onClick;
        _onHold    = onHold;
        _onRelease = onRelease;

        Center  = center;
        Extents = extents;
    }

    Button::Button(const Vector2i& center, const Vector2i& extents, ScaleMode scaleMode,
                   const std::optional<Callback>& onEnter,
                   const std::optional<Callback>& onInside,
                   const std::optional<Callback>& onLeave,
                   const std::optional<Callback>& onOutside,
                   const std::optional<Callback>& onClick,
                   const std::optional<Callback>& onHold,
                   const std::optional<Callback>& onRelease)
    {
        *this = Button(ConvertRetroScreenCoordsToScreenPosition(center),
                       ConvertRetroScreenCoordsToScreenPosition(extents),
                       scaleMode,
                       onEnter,
                       onInside,
                       onLeave,
                       onOutside,
                       onClick,
                       onHold,
                       onRelease);
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

    void Button::Update(const Vector2& pos)
    {
        static const auto SELECT_ACTION_IDS = std::vector<ActionId>
        {
            In::MouseClickLeft
        };

        auto correctedExtents = Extents * GetScreenAspectCorrection(_scaleMode);
        auto bounds           = AxisAlignedBoundingRect(Center, correctedExtents);
        Update(bounds.Intersects(pos), SELECT_ACTION_IDS); 
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

    void Button::Update(bool activeState, const std::vector<ActionId>& selectActionIds)
    {
        if (activeState)
        {
            // Execute `_onEnter` callback if previous state was inactive.
            if (!_prevActiveState)
            {
                ExecuteCallback(_onEnter);
            }
            // Execute `_onInside` callback if previous state was also active.
            else
            {
                ExecuteCallback(_onInside);
            }

            // Execute `_onClick`, `_onHold`, or `_onRelease` callback if select action is registered.
            if (CheckClickedAction(selectActionIds))
            {
                ExecuteCallback(_onClick);
            }
            else if (CheckHeldAction(selectActionIds))
            {
                ExecuteCallback(_onHold);
            }
            else if (CheckReleasedAction(selectActionIds))
            {
                ExecuteCallback(_onRelease);
            }
        }
        else
        {
            // Execute `_onLeave` callback if previous state was active.
            if (_prevActiveState)
            {
                ExecuteCallback(_onLeave);
            }
            // Execute `_onOutside` callback if previous state was also inactive.
            else
            {
                ExecuteCallback(_onOutside);
            }
        }

        _prevActiveState = activeState;
    }

    void Button::ExecuteCallback(const std::optional<Callback>& callback) const
    {
        if (callback.has_value())
        {
            (*callback)();
        }
    }
}
