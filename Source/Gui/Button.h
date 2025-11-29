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

        bool _prevActiveState  = false;

        ScaleMode               _scaleMode = ScaleMode::Fit;
        std::optional<Callback> _onEnter   = std::nullopt;
        std::optional<Callback> _onInside  = std::nullopt;
        std::optional<Callback> _onLeave   = std::nullopt;
        std::optional<Callback> _onOutside = std::nullopt;
        std::optional<Callback> _onClick   = std::nullopt;
        std::optional<Callback> _onHold    = std::nullopt;
        std::optional<Callback> _onRelease = std::nullopt;

    public:
        AxisAlignedBoundingRect Bounds = AxisAlignedBoundingRect();

        // =============
        // Constructors
        // =============

        /** @brief Constructs a `Button` from interaction bounds and callbacks.
         *
         * @param bounds Interaction bounds.
         * @param scaleMode Interaction bounds scale mode.
         * @param onEnter Interaction callback to execute when entering.
         * @param onInside Interaction callback to execute when inside.
         * @param onLeave Interaction callback to execute when leaving.
         * @param onOutside Interaction callback to execute when outside.
         * @param onClick Interaction callback to execute when input is clicked.
         * @param onHold Interaction callback to execute when input is held.
         * @param onRelease Interaction callback to execute when input is released.
         */
        Button(const AxisAlignedBoundingRect& bounds, ScaleMode scaleMode,
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
         * @param actionIds IDs of input actions to checks
         * @return `true` if an action is clicked, `false` otherwise.
         */
        bool CheckClickedAction(const std::vector<ActionId>& actionIds) const;

        /** @brief Checks if an input action in a given list is held.
         *
         * @param actionIds IDs of input actions to checks
         * @return `true` if an action is held, `false` otherwise.
         */
        bool CheckHeldAction(const std::vector<ActionId>& actionIds) const;

        /** @brief Checks if an input action in a given list is released.
         *
         * @param actionIds IDs of input actions to checks
         * @return `true` if an action is released, `false` otherwise.
         */
        bool CheckReleasedAction(const std::vector<ActionId>& actionIds) const;

        /** @brief Updates the button state using a screen space point from mouse or controller cursor navigation and executes callbacks.
         *
         * @param activeState `true` if the button is active, `false` otherwise.
         * @param selectActionIds Input action IDs used for selection.
         */
        void Update(bool activeState, const std::vector<ActionId>& selectActionIds);

        /** @brief Executes an interaction callback.
         *
         * @param callback Interaction callback.
         */
        void ExecuteCallback(const std::optional<Callback>& callback) const;
    };
}
