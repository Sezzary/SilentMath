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

        std::optional<Callback> _onEnter   = std::nullopt;
        std::optional<Callback> _onInside  = std::nullopt;
        std::optional<Callback> _onLeave   = std::nullopt;
        std::optional<Callback> _onOutside = std::nullopt;
        std::optional<Callback> _onClick   = std::nullopt;
        std::optional<Callback> _onHold    = std::nullopt;
        std::optional<Callback> _onRelease = std::nullopt;

    public:
        // =============
        // Constructors
        // =============

        /** @brief Constructs a `Button` with bounds and interaction callbacks.
         *
         * @param bounds 2D rectangle representing interaction bounds.
         * @param scaleMode Renderer scale mode.
         * @param onEnter Callback to execute when entering.
         * @param onInside Callback to execute when inside.
         * @param onLeave Callback to execute when leaving.
         * @param onOutside Callback to execute when outside.
         * @param onClick Callback to execute when input is clicked.
         * @param onHold Callback to execute when input is held.
         * @param onRelease Callback to execute when input is released.
         */
        Button(const Rectangle& bounds, ScaleMode scaleMode,
               const std::optional<Callback>& onEnter,
               const std::optional<Callback>& onInside,
               const std::optional<Callback>& onLeave,
               const std::optional<Callback>& onOutside,
               const std::optional<Callback>& onClick,
               const std::optional<Callback>& onHold,
               const std::optional<Callback>& onRelease);

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
