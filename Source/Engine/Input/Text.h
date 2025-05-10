#pragma once

#include "Engine/Input/Action.h"

namespace Silent::Input
{
    struct TextBufferData
    {
        std::string                          Text          = {};
        std::string                          PreviewText   = {};
        uint                                 Cursor        = 0;
        std::vector<ActionId>                PrevActionIds = {};
        std::optional<std::pair<uint, uint>> Selection     = {};
    };

    class TextManager
    {
    private:
        // Constants

        static constexpr float PULSE_DELAY_SEC         = 0.1f;
        static constexpr float PULSE_INITIAL_DELAY_SEC = 0.4f;

        // Fields

        std::unordered_map<std::string, TextBufferData> _buffers = {}; // Key = ID, value = text buffer.

    public:
        // Constructors

        TextManager() = default;

        // Getters
        
        const std::string& GetText(const std::string& bufferId) const;
        uint               GetCursorPosition(const std::string& bufferId) const;
        
        // Utilities

        void UpdateBuffer(const std::string& bufferId, const std::unordered_map<ActionId, Action>& actions);
        void ClearBuffer(const std::string& bufferId);

    private:
        // Helpers

        bool HandleCursorMove(const std::string& bufferId, const std::unordered_map<ActionId, Action>& actions);
        bool HandleCharacterClear(const std::string& bufferId, const std::unordered_map<ActionId, Action>& actions);
        bool HandleCharacterAdd(const std::string& bufferId, const std::unordered_map<ActionId, Action>& actions);
    };
}
