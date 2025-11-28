#include "Framework.h"
#include "Gui/Button.h"

#include "Input/Input.h"

using namespace Silent::Input;

namespace Silent::Gui
{
    Button::Button(const Rectangle& bounds,
                   std::optional<Callback> onEnter,
                   std::optional<Callback> onInside,
                   std::optional<Callback> onLeave,
                   std::optional<Callback> onClick,
                   std::optional<Callback> onHold,
                   std::optional<Callback> onRelease)
    {
        _bounds    = bounds;
        _onEnter   = onEnter.value_or(nullptr);
        _onInside  = onInside.value_or(nullptr);
        _onLeave   = onLeave.value_or(nullptr);
        _onClick   = onClick.value_or(nullptr);
        _onHold    = onHold.value_or(nullptr);
        _onRelease = onRelease.value_or(nullptr);
    }

    void Button::Update(const Vector2& point)
    {
        if (!_bounds.Intersects(point))
        {
            return;
        }
    }

    void Button::HandleEnter(const Vector2& point) const
    {
        // Check if hover is valid.
        if (!_onEnter)
        {
            return;
        }

        _onEnter();
    }

    void Button::HandleClick(const Vector2& point) const
    {
        if (!_onClick)
        {
            return;
        }

        _onClick();
    }
}
