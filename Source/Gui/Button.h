#pragma once

namespace Silent::Gui
{
    /** @brief Clickable screen-space region. */
    class Button
    {
    private:
        // ========
        // Aliases
        // ========

        using Callback = std::function<void()>;

        // =======
        // Fields
        // =======

        Rectangle _bounds    = Rectangle();
        Callback  _onEnter   = nullptr;
        Callback  _onInside  = nullptr;
        Callback  _onLeave   = nullptr;
        Callback  _onClick   = nullptr;
        Callback  _onHold    = nullptr;
        Callback  _onRelease = nullptr;

        bool _isInside  = false;
        bool _isOutside = false;

    public:
        // =============
        // Constructors
        // =============

        Button(const Rectangle& bounds,
               std::optional<Callback> onEnter,
               std::optional<Callback> onInside,
               std::optional<Callback> onLeave,
               std::optional<Callback> onClick,
               std::optional<Callback> onHold,
               std::optional<Callback> onRelease);

        // ==========
        // Utilities
        // ==========

        void Update(const Vector2& point);

    private:
        // ========
        // Helpers
        // ========

        void HandleEnter(const Vector2& point) const;

        void HandleClick(const Vector2& point) const;
    };
}
