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

        bool _prevState  = false;

        ScaleMode               _scaleMode = ScaleMode::Fit;
        std::optional<Callback> _onEnter   = std::nullopt;
        std::optional<Callback> _onInside  = std::nullopt;
        std::optional<Callback> _onLeave   = std::nullopt;
        std::optional<Callback> _onOutside = std::nullopt;
        std::optional<Callback> _onClick   = std::nullopt;
        std::optional<Callback> _onHold    = std::nullopt;
        std::optional<Callback> _onRelease = std::nullopt;

    public:
        Vector2 Center  = Vector2::Zero;
        Vector2 Extents = Vector2::Zero;

        // =============
        // Constructors
        // =============

        /** @brief Constructs a `Button` from a center and extents in screen percent with callbacks.
         *
         * @param center Center in screen percent.
         * @param extents Extents in screen percent.
         * @param scaleMode Extents scale mode relative to screen aspect ratio.
         * @param onEnter Interaction callback to execute when entering.
         * @param onInside Interaction callback to execute when inside.
         * @param onLeave Interaction callback to execute when leaving.
         * @param onOutside Interaction callback to execute when outside.
         * @param onClick Interaction callback to execute when input is clicked.
         * @param onHold Interaction callback to execute when input is held.
         * @param onRelease Interaction callback to execute when input is released.
         */
        Button(const Vector2& center, const Vector2& extents, ScaleMode scaleMode,
               const std::optional<Callback>& onEnter,
               const std::optional<Callback>& onInside,
               const std::optional<Callback>& onLeave,
               const std::optional<Callback>& onOutside,
               const std::optional<Callback>& onClick,
               const std::optional<Callback>& onHold,
               const std::optional<Callback>& onRelease);

        /** @brief Constructs a `Button` from a center and extents in retro screen coordinates with callbacks.
         *
         * @param center Screen space center in retro screen coordinates (320x240).
         * @param extents Screen space extents in retro screen coordinates (320x240).
         * @param scaleMode Extents scale mode relative to screen aspect ratio.
         * @param onEnter Interaction callback to execute when entering.
         * @param onInside Interaction callback to execute when inside.
         * @param onLeave Interaction callback to execute when leaving.
         * @param onOutside Interaction callback to execute when outside.
         * @param onClick Interaction callback to execute when input is clicked.
         * @param onHold Interaction callback to execute when input is held.
         * @param onRelease Interaction callback to execute when input is released.
         */
        Button(const Vector2i& center, const Vector2i& extents, ScaleMode scaleMode,
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

        /** @brief Updates the button state using a discrete focus state from input navigation and executes callbacks.
         *
         * @param isFocused `true` if the button is focused, `false` otherwise.
         */
        void Update(bool isFocused);

        /** @brief Updates the button state using a screen space position from cursor navigation and executes callbacks.
         *
         * @param pos Screen space cursor position.
         */
        void Update(const Vector2& pos);

        /** @brief Displays debug information in the power menu. */
        void Debug() const;

    private:
        // ========
        // Helpers
        // ========

        /** @brief Checks if an input action in a given list is clicked.
         *
         * @param actionIds IDs of input actions to check.
         * @return `true` if an action is clicked, `false` otherwise.
         */
        bool IsActionClicked(const std::vector<ActionId>& actionIds) const;

        /** @brief Checks if an input action in a given list is held.
         *
         * @param actionIds IDs of input actions to check.
         * @return `true` if an action is held, `false` otherwise.
         */
        bool IsActionHeld(const std::vector<ActionId>& actionIds) const;

        /** @brief Checks if an input action in a given list is released.
         *
         * @param actionIds IDs of input actions to check.
         * @return `true` if an action is released, `false` otherwise.
         */
        bool IsActionReleased(const std::vector<ActionId>& actionIds) const;

        /** @brief Updates the button state using a screen space point from mouse or controller cursor navigation and executes callbacks.
         *
         * @param state `true` if the button is active, `false` otherwise.
         * @param selectActionIds Input action IDs used for selection.
         */
        void Update(bool state, const std::vector<ActionId>& selectActionIds);

        /** @brief Executes an interaction callback.
         *
         * @param callback Interaction callback.
         */
        void ExecuteCallback(const std::optional<Callback>& callback) const;
    };
}
