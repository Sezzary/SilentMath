#pragma once

#include "Engine/Input/Action.h"

namespace Silent::Input
{
    class TextManager
    {
    private:
        // Constants

        static constexpr float PULSE_DELAY_SEC         = 0.1f;
        static constexpr float PULSE_INITIAL_DELAY_SEC = 0.4f;

        // Fields

        std::unordered_map<std::string, std::string>           _buffers       = {}; // Key = ID, value = buffer
        std::unordered_map<std::string, unsigned int>          _cursors       = {}; // Key = ID, value = position cursor.
        std::unordered_map<std::string, std::vector<ActionId>> _prevActionIds = {}; // Key = ID, value = previous action IDs.

    public:
        // Constructors

        TextManager() = default;

        // Getters
        
        const std::string& GetTextBuffer(const std::string& bufferId) const;
        
        // Utilities

        void UpdateTextBuffer(const std::string& bufferId, const std::unordered_map<ActionId, Action>& actions);
        void ClearTextBuffer(const std::string& bufferId);

    private:
        // Helpers

        void HandleCursorMove();
        void HandleCharacterClear();
    };
}
