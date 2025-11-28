#pragma once

#include "Input/Input.h"
#include "Renderer/Common/Enums.h"

using namespace Silent::Input;
using namespace Silent::Renderer;

namespace Silent::Gui
{
    /** @brief Interactable screen space region. */
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

        bool _isActive  = false;

        Rectangle _bounds    = Rectangle();
        ScaleMode _scaleMode = ScaleMode::Fit;

        Callback  _onEnter   = nullptr;
        Callback  _onInside  = nullptr;
        Callback  _onLeave   = nullptr;
        Callback  _onOutside = nullptr;
        Callback  _onClick   = nullptr;
        Callback  _onHold    = nullptr;
        Callback  _onRelease = nullptr;

    public:
        // =============
        // Constructors
        // =============

        /** @brief Constructs a `Button` with bounds and interaction callbacks.
         *
         * @param bounds 2D rectangle representing interaction bounds.
         * @param scaleMode Renderer scale mode.
         * @param onEnter Callback to execute on enter.
         * @param onInside Callback to execute on inside.
         * @param onLeave Callback to execute on leave.
         * @param onOutside Callback to execute on outside.
         * @param onClick Callback to execute on clicked input.
         * @param onHold Callback to execute on held input.
         * @param onRelease Callback to execute on released input.
         */
        Button(const Rectangle& bounds, Silent::Renderer::ScaleMode scaleMode,
               std::optional<Callback> onEnter,
               std::optional<Callback> onInside,
               std::optional<Callback> onLeave,
               std::optional<Callback> onOutside,
               std::optional<Callback> onClick,
               std::optional<Callback> onHold,
               std::optional<Callback> onRelease);

        // ==========
        // Utilities
        // ==========

        /** @brief Updates the button state using a discrete focus state from keyboard or controller button navigation and executes callbacks.
         *
         * @param isFocused `true` if the button is focused, `false` otherwise.
         */
        void Update(bool isFocused);

        /** @brief Updates the button state using a screen space point from mouse or controller cursor navigation and executes callbacks.
         *
         * @param point Screen space point.
         */
        void Update(const Vector2& point);

    private:
        // ========
        // Helpers
        // ========

        /** @brief Checks if an input action in a given list is clicked.
         *
         * @return `true` if an action is clicked, `false` otherwise.
         */
        bool CheckClickedAction(const std::span<const ActionId>& actionIds) const;

        /** @brief Checks if an input action in a given list is held.
         *
         * @return `true` if an action is held, `false` otherwise.
         */
        bool CheckHeldAction(const std::span<const ActionId>& actionIds) const;

        /** @brief Checks if an input action in a given list is released.
         *
         * @return `true` if an action is released, `false` otherwise.
         */
        bool CheckReleasedAction(const std::span<const ActionId>& actionIds) const;

        /** @brief Updates the button state using a screen space point from mouse or controller cursor navigation and executes callbacks.
         *
         * @param isActive `true` if the button is active, `false` otherwise.
         * @param selectActionIds Input action IDs used for selection.
         */
        void Update(bool isActive, const std::span<const ActionId>& selectActionIds);

        /** @brief Executes an interaction callback.
         *
         * @param callback Interaction callback.
         */
        void ExecuteCallback(const std::optional<Callback>& callback) const;
    };
}
