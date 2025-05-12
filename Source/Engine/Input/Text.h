#pragma once

#include "Engine/Input/Action.h"

namespace Silent::Input
{
    struct TextBuffer
    {
        std::string                          Text      = {};
        std::optional<std::pair<uint, uint>> Selection = {}; // First = selection start, second = selection end.
        std::deque<std::string>              Undo      = {};
        std::deque<std::string>              Redo      = {};
        uint                                 Cursor    = 0;

        std::vector<uint> LineStarts   = {};
        uint              LineWidthMax = 0;
        uint              CharCountMax = 0;
    };

    class TextManager
    {
    private:
        // Constants

        static constexpr float PULSE_DELAY_SEC         = 0.1f;
        static constexpr float PULSE_INITIAL_DELAY_SEC = 0.3f;
        static constexpr uint  HISTORY_SIZE_MAX        = 128;

        // Fields

        std::unordered_map<std::string, TextBuffer> _buffers       = {}; // Key = ID, value = text buffer.
        std::string                                 _clipboard     = {};
        std::vector<ActionId>                       _prevActionIds = {};

    public:
        // Constructors

        TextManager() = default;

        // Getters

        const std::string&       GetText(const std::string& bufferId) const;
        std::vector<std::string> GetTextLines(const std::string& bufferId) const;
        uint                     GetCursorPosition(const std::string& bufferId) const;
        
        // Utilities

        void InsertBuffer(const std::string& bufferId, uint lineWidthMax, uint charCountMax);
        void UpdateBuffer(const std::string& bufferId, const std::unordered_map<ActionId, Action>& actions);
        void RemoveBuffer(const std::string& bufferId);

    private:
        // Helpers

        bool HandleHistory(TextBuffer& buffer, const std::unordered_map<ActionId, Action>& actions);
        bool HandleClipboard(TextBuffer& buffer, const std::unordered_map<ActionId, Action>& actions);
        bool HandleCharacterAdd(TextBuffer& buffer, const std::unordered_map<ActionId, Action>& actions);
        bool HandleCharacterClear(TextBuffer& buffer, const std::unordered_map<ActionId, Action>& actions);
        bool HandleCursorSelection(TextBuffer& buffer, const std::unordered_map<ActionId, Action>& actions);

        void PushUndo(TextBuffer& buffer);
        void UpdateLineStarts(TextBuffer& buffer);
    };
}
