#include "Framework.h"
#include "Engine/Input/Text.h"

#include "Engine/Input/Action.h"
#include "Utils/Utils.h"

using namespace Silent::Utils;

namespace Silent::Input
{
    static const auto PRINTABLE_ACTION_CHARS = std::unordered_map<ActionId, std::pair<char, char>>
    {
        { In::A,            { 'a', 'A' } },
        { In::B,            { 'b', 'B' } },
        { In::C,            { 'c', 'C' } },
        { In::D,            { 'd', 'D' } },
        { In::E,            { 'e', 'E' } },
        { In::F,            { 'f', 'F' } },
        { In::G,            { 'g', 'G' } },
        { In::H,            { 'h', 'H' } },
        { In::I,            { 'i', 'I' } },
        { In::J,            { 'j', 'J' } },
        { In::K,            { 'k', 'K' } },
        { In::L,            { 'l', 'L' } },
        { In::M,            { 'm', 'M' } },
        { In::N,            { 'n', 'N' } },
        { In::O,            { 'o', 'O' } },
        { In::P,            { 'p', 'P' } },
        { In::Q,            { 'q', 'Q' } },
        { In::R,            { 'r', 'R' } },
        { In::S,            { 's', 'S' } },
        { In::T,            { 't', 'T' } },
        { In::U,            { 'u', 'U' } },
        { In::V,            { 'v', 'V' } },
        { In::W,            { 'w', 'W' } },
        { In::X,            { 'x', 'X' } },
        { In::Y,            { 'y', 'Y' } },
        { In::Z,            { 'z', 'Z' } },
        { In::Num1,         { '1', '!' } },
        { In::Num2,         { '2', '@' } },
        { In::Num3,         { '3', '#' } },
        { In::Num4,         { '4', '$' } },
        { In::Num5,         { '5', '%' } },
        { In::Num6,         { '6', '^' } },
        { In::Num7,         { '7', '&' } },
        { In::Num8,         { '8', '*' } },
        { In::Num9,         { '9', '(' } },
        { In::Num0,         { '0', ')' } },
        { In::Space,        { ' ', ' ' } },
        { In::Minus,        { '-', '_' } },
        { In::Equals,       { '=', '+' } },
        { In::BracketLeft,  { '[', '{' } },
        { In::BracketRight, { ']', '}' } },
        { In::Backslash,    { '\\', '|' } },
        { In::Semicolon,    { ';', ':' } },
        { In::Apostrophe,   { '\'', '"' } },
        { In::Comma,        { ',', '<' } },
        { In::Period,       { '.', '>' } },
        { In::Slash,        { '/', '?' } }
    };

    const std::string& TextManager::GetText(const std::string& bufferId) const
    {
        static const auto EMPTY = std::string();

        auto it = _buffers.find(bufferId);
        if (it == _buffers.end())
        {
            Log("Attempted to get text for missing text buffer '" + bufferId + "'.", LogLevel::Warning);
            return EMPTY;
        }

        const auto& [keyId, buffer] = *it;
        return buffer.Text;
    }

    uint TextManager::GetCursorPosition(const std::string& bufferId) const
    {
        auto it = _buffers.find(bufferId);
        if (it == _buffers.end())
        {
            Log("Attempted to get text cursor position for missing text buffer '" + bufferId + "'.", LogLevel::Warning);
            return 0;
        }

        const auto& [keyId, buffer] = *it;
        return buffer.Cursor;
    }

    void TextManager::UpdateBuffer(const std::string& bufferId, uint lengthMax, const std::unordered_map<ActionId, Action>& actions)
    {
        auto it = _buffers.find(bufferId);
        if (it == _buffers.end())
        {
            Log("Attempted to update missing text buffer '" + bufferId + "'.", LogLevel::Warning);
            return;
        }

        auto& [keyId, buffer] = *it;

        // Move cursor.
        if (HandleCursorSelection(buffer, actions))
        {
            return;
        }

        // Clear characters.
        if (HandleCharacterClear(buffer, actions))
        {
            return;
        }

        // Add character.
        if (HandleCharacterAdd(buffer, lengthMax, actions))
        {
            return;
        }

        // Undo/redo.
        if (HandleHistory(buffer, actions))
        {
            return;
        }
    }

    void TextManager::ClearBuffer(const std::string& bufferId)
    {
        auto it = _buffers.find(bufferId);
        if (it == _buffers.end())
        {
            Log("Attempted to clear missing text buffer '" + bufferId + "'.", LogLevel::Warning);
            return;
        }

        _buffers.erase(bufferId);
    }

    bool TextManager::HandleCursorSelection(TextBuffer& buffer, const std::unordered_map<ActionId, Action>& actions)
    {
        const auto& leftAction  = actions.at(In::ArrowLeft);
        const auto& rightAction = actions.at(In::ArrowRight);
        const auto& shiftAction = actions.at(In::Shift);
        const auto& ctrlAction  = actions.at(In::Ctrl);
        const auto& aAction     = actions.at(In::A);

        // Select all.
        if (ctrlAction.IsHeld() && aAction.IsClicked())
        {
            buffer.Selection = std::pair(0, buffer.Text.size());
            buffer.Cursor    = buffer.Text.size();
            return true;
        }

        // Move left/right.
        if (leftAction.IsHeld() || rightAction.IsHeld())
        {
            if (buffer.Cursor > 0 && leftAction.IsPulsed(PULSE_DELAY_SEC, PULSE_INITIAL_DELAY_SEC))
            {
                uint prevCursor = buffer.Cursor;

                // Move back to previous word.
                if (ctrlAction.IsHeld())
                {
                    uint prevCursor = buffer.Cursor;

                    // Skip spaces before word.
                    while (buffer.Cursor > 0 && buffer.Text[buffer.Cursor - 1] == ' ')
                    {
                        buffer.Cursor--;
                    }

                    // Skip word.
                    while (buffer.Cursor > 0 && buffer.Text[buffer.Cursor - 1] != ' ')
                    {
                        buffer.Cursor--;
                    }

                }
                // Move back to previous character.
                else
                {
                    buffer.Cursor--;
                }

                // Expand selection back.
                if (shiftAction.IsHeld())
                {
                    if (buffer.Selection.has_value())
                    {
                        buffer.Selection->first = buffer.Cursor;
                    }
                    else
                    {
                        buffer.Selection = std::pair(buffer.Cursor, prevCursor);
                    }
                }
            }
            else if (buffer.Cursor < buffer.Text.size() && rightAction.IsPulsed(PULSE_DELAY_SEC, PULSE_INITIAL_DELAY_SEC))
            {
                uint prevCursor = buffer.Cursor;

                // Move forward to next word.
                if (ctrlAction.IsHeld())
                {
                    // Skip current word.
                    while (buffer.Cursor < buffer.Text.size() && buffer.Text[buffer.Cursor] != ' ')
                    {
                        buffer.Cursor++;
                    }

                    // Skip spaces after word.
                    while (buffer.Cursor < buffer.Text.size() && buffer.Text[buffer.Cursor] == ' ')
                    {
                        buffer.Cursor++;
                    }
                }
                // More forward to next character.
                else
                {
                    buffer.Cursor++;
                }

                // Expand selection forward.
                if (shiftAction.IsHeld())
                {
                    if (buffer.Selection.has_value())
                    {
                        buffer.Selection->second = buffer.Cursor;
                    }
                    else
                    {
                        buffer.Selection = std::pair(prevCursor, buffer.Cursor);
                    }
                }
            }

            return true;
        }

        return false;
    }

    bool TextManager::HandleCharacterClear(TextBuffer& buffer, const std::unordered_map<ActionId, Action>& actions)
    {
        const auto& shiftAction = actions.at(In::Shift);
        const auto& ctrlAction  = actions.at(In::Ctrl);
        const auto& bsAction    = actions.at(In::Backspace);
        const auto& delAction   = actions.at(In::Delete);

        if (bsAction.IsHeld())
        {
            if (!buffer.Text.empty() && bsAction.IsPulsed(PULSE_DELAY_SEC, PULSE_INITIAL_DELAY_SEC))
            {
                PushUndo(buffer);

                // Erase selection.
                if (buffer.Selection.has_value() && ctrlAction.IsHeld())
                {
                    auto start = buffer.Text.begin() + buffer.Selection->first;
                    auto end   = buffer.Text.begin() + buffer.Selection->second;
                    buffer.Text.erase(start, end);

                    buffer.Cursor    = buffer.Selection->first;
                    buffer.Selection = std::nullopt;
                }
                // Erase back to start.
                else if (shiftAction.IsHeld() && ctrlAction.IsHeld())
                {
                    buffer.Text.erase(0, buffer.Cursor);
                    buffer.Cursor = 0;
                }
                // Erase back to previous space.
                else if (ctrlAction.IsHeld())
                {
                    char curChar = 0;
                    while (buffer.Cursor > 0 && curChar != ' ')
                    {
                        buffer.Text.erase(buffer.Text.begin() + (buffer.Cursor - 1));

                        buffer.Cursor--;
                        if (buffer.Cursor > 0)
                        {
                            curChar = buffer.Text.at(buffer.Cursor - 1);
                        }
                    }
                }
                // Erase back single character.
                else
                {
                    if (buffer.Cursor > 0)
                    {
                        buffer.Text.erase(buffer.Text.begin() + (buffer.Cursor - 1));
                        buffer.Cursor--;
                    }
                }
            }

            return true;
        }
        else if (!buffer.Text.empty() && delAction.IsHeld())
        {
            if (!buffer.Text.empty() && delAction.IsPulsed(PULSE_DELAY_SEC, PULSE_INITIAL_DELAY_SEC))
            {
                PushUndo(buffer);

                // Erase selection.
                if (buffer.Selection.has_value() && ctrlAction.IsHeld())
                {
                    auto start = buffer.Text.begin() + buffer.Selection->first;
                    auto end   = buffer.Text.begin() + buffer.Selection->second;
                    buffer.Text.erase(start, end);

                    buffer.Cursor    = buffer.Selection->first;
                    buffer.Selection = std::nullopt;
                }
                // Erase forward to end.
                else if (shiftAction.IsHeld() && ctrlAction.IsHeld())
                {
                    buffer.Text.erase(buffer.Cursor, buffer.Text.size() - buffer.Cursor);
                }
                // Erase forward to next space.
                else if (ctrlAction.IsHeld())
                {
                    char curChar = 0;
                    while (buffer.Cursor < buffer.Text.size() && curChar != ' ')
                    {
                        buffer.Text.erase(buffer.Text.begin() + buffer.Cursor);

                        if (buffer.Cursor < buffer.Text.size())
                        {
                            curChar = buffer.Text.at(buffer.Cursor);
                        }
                    }
                }
                // Erase forward single character.
                else
                {
                    if (buffer.Cursor < buffer.Text.size())
                    {
                        buffer.Text.erase(buffer.Text.begin() + buffer.Cursor);
                    }
                }
            }

            return true;
        }

        return false;
    }

    bool TextManager::HandleCharacterAdd(TextBuffer& buffer, uint lengthMax, const std::unordered_map<ActionId, Action>& actions)
    {
        const auto& shiftAction = actions.at(In::Shift);

        if (lengthMax >= buffer.Text.size())
        {
            return false;
        }

        bool hasNewChar = false;
        for (auto actionId : PRINTABLE_ACTION_IDS)
        {
            const auto& action = actions.at(actionId);
            const auto& chars  = PRINTABLE_ACTION_CHARS.at(actionId);

            if (!hasNewChar && action.IsHeld())
            {
                // New action.
                if (!Contains(ToSpan(_prevActionIds), actionId))
                {
                    // Add character.
                    if (action.IsClicked())
                    {
                        PushUndo(buffer);

                        buffer.Text.insert(buffer.Text.begin() + buffer.Cursor, shiftAction.IsHeld() ? chars.second : chars.first);
                        buffer.Cursor++;
                    }

                    _prevActionIds.push_back(actionId);
                    hasNewChar = true;
                }
                // Held action.
                else if (!_prevActionIds.empty() && _prevActionIds.back() == actionId)
                {
                    // Add character.
                    if (action.IsPulsed(PULSE_DELAY_SEC, PULSE_INITIAL_DELAY_SEC))
                    {
                        PushUndo(buffer);

                        buffer.Text.insert(buffer.Text.begin() + buffer.Cursor, shiftAction.IsHeld() ? chars.second : chars.first);
                        buffer.Cursor++;
                        hasNewChar = true;
                    }
                }
            }
            else if (action.IsReleased())
            {
                std::erase(_prevActionIds, actionId);
            }
        }

        return hasNewChar;
    }

    bool TextManager::HandleCutCopyPaste(TextBuffer& buffer, uint lengthMax, const std::unordered_map<ActionId, Action>& actions)
    {
        if (_clipboard.empty() && !buffer.Selection.has_value())
        {
            return false;
        }

        const auto& ctrlAction = actions.at(In::Ctrl);
        const auto& xAction    = actions.at(In::X);
        const auto& cAction    = actions.at(In::C);
        const auto& vAction    = actions.at(In::V);

        if (ctrlAction.IsHeld())
        {
            // Cut selection.
            if (xAction.IsClicked())
            {
                if (buffer.Selection.has_value())
                {
                    auto start = buffer.Text.begin() + buffer.Selection->first;
                    auto end   = buffer.Text.begin() + buffer.Selection->second;
                    _clipboard = std::string(start, end);
                    buffer.Text.erase(start, end);

                    buffer.Cursor    = buffer.Selection->first;
                    buffer.Selection = std::nullopt;
                    return true;
                }
            }
            // Copy selection.
            else if (cAction.IsClicked())
            {
                if (buffer.Selection.has_value())
                {
                    auto start = buffer.Text.begin() + buffer.Selection->first;
                    auto end   = buffer.Text.begin() + buffer.Selection->second;
                    _clipboard = std::string(start, end);
                    return true;
                }
            }
            // Paste copy.
            else if (vAction.IsClicked())
            {
                if (!_clipboard.empty() && (buffer.Text.size() + _clipboard.size()) <= lengthMax)
                {
                    // Replace selection.
                    if (buffer.Selection.has_value())
                    {
                        uint selectLength = buffer.Selection->second - buffer.Selection->first;
                        if (((buffer.Text.size() + _clipboard.size()) - selectLength) <= lengthMax)
                        {
                            auto start = buffer.Text.begin() + buffer.Selection->first;
                            auto end   = buffer.Text.begin() + buffer.Selection->second;
                            buffer.Text.erase(start, end);
                            buffer.Text.insert(buffer.Selection->first, _clipboard);

                            buffer.Cursor    = buffer.Selection->first + _clipboard.size();
                            buffer.Selection = std::nullopt;
                            return true;
                        }
                    }
                    // Insert at cursor.
                    else
                    {
                        buffer.Text.insert(buffer.Cursor, _clipboard);
                        buffer.Cursor += _clipboard.size();
                        return true;
                    }
                }
            }
        }

        return false;
    }

    bool TextManager::HandleHistory(TextBuffer& buffer, const std::unordered_map<ActionId, Action>& actions)
    {
        const auto& shiftAction = actions.at(In::Shift);
        const auto& ctrlAction  = actions.at(In::Ctrl);
        const auto& zAction     = actions.at(In::Z);

        if (!ctrlAction.IsHeld() || !zAction.IsHeld())
        {
            return false;
        }

        if (zAction.IsPulsed(PULSE_INITIAL_DELAY_SEC, PULSE_DELAY_SEC))
        {
            auto& fromStack = shiftAction.IsHeld() ? buffer.Redo : buffer.Undo;
            auto& toStack   = shiftAction.IsHeld() ? buffer.Undo : buffer.Redo;

            if (!fromStack.empty())
            {
                toStack.push_back(buffer.Text);
                if (toStack.size() > HISTORY_SIZE_MAX)
                {
                    toStack.pop_front();
                }

                buffer.Text = fromStack.back();
                fromStack.pop_back();
            }
        }

        return true;
    }

    void TextManager::PushUndo(TextBuffer& buffer)
    {
        if (buffer.Undo.size() >= HISTORY_SIZE_MAX)
        {
            buffer.Undo.pop_front();
        }
        buffer.Undo.push_back(buffer.Text);

        buffer.Redo.clear();
    }
}
