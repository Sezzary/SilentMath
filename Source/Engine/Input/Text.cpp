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

    const std::string& TextManager::GetBuffer(const std::string& bufferId) const
    {
        static const auto EMPTY = std::string();

        auto it = _buffers.find(bufferId);
        if (it == _buffers.end())
        {
            return EMPTY;
        }

        const auto& [keyId, buffer] = *it;
        return buffer;
    }

    void TextManager::UpdateBuffer(const std::string& bufferId, const std::unordered_map<ActionId, Action>& actions)
    {
        // Move cursor.
        if (HandleCursorMove(bufferId, actions))
        {
            return;
        }

        // Clear characters.
        if (HandleCharacterClear(bufferId, actions))
        {
            return;
        }

        // Add character.
        if (HandleCharacterAdd(bufferId, actions))
        {
            return;
        }
    }

    void TextManager::ClearBuffer(const std::string& bufferId)
    {
        _buffers.erase(bufferId);
        _cursors.erase(bufferId);
        _prevActionIds.erase(bufferId);
    }
    
    bool TextManager::HandleCursorMove(const std::string& bufferId, const std::unordered_map<ActionId, Action>& actions)
    {
        auto& buffer = _buffers[bufferId];
        uint& cursor = _cursors[bufferId];

        auto& leftAction  = actions.at(In::ArrowLeft);
        auto& rightAction = actions.at(In::ArrowRight);

        if (leftAction.IsHeld() || rightAction.IsHeld())
        {
            if (leftAction.IsPulsed(PULSE_DELAY_SEC, PULSE_INITIAL_DELAY_SEC))
            {
                if (cursor > 0)
                {
                    cursor--;
                }
            }
            else if (rightAction.IsPulsed(PULSE_DELAY_SEC, PULSE_INITIAL_DELAY_SEC))
            {
                if (cursor < (buffer.size() + 1))
                {
                    cursor++;
                }
            }

            return true;
        }

        return false;
    }

    bool TextManager::HandleCharacterClear(const std::string& bufferId, const std::unordered_map<ActionId, Action>& actions)
    {
        auto& buffer = _buffers[bufferId];
        uint& cursor = _cursors[bufferId];

        const auto& bsAction    = actions.at(In::Backspace);
        const auto& delAction   = actions.at(In::Delete);
        const auto& shiftAction = actions.at(In::Shift);
        const auto& ctrlAction  = actions.at(In::Ctrl);

        if (bsAction.IsHeld())
        {
            if (!buffer.empty() && bsAction.IsPulsed(PULSE_DELAY_SEC, PULSE_INITIAL_DELAY_SEC))
            {
                // Erase back to start.
                if (shiftAction.IsHeld() && ctrlAction.IsHeld())
                {
                    while (cursor > 0)
                    {
                        buffer.erase(0, cursor);
                        cursor = 0;
                    }
                }
                // Erase back to previous space.
                else if (ctrlAction.IsHeld())
                {
                    char curChar = '.';
                    while (cursor > 0 && curChar != ' ')
                    {
                        buffer.erase(buffer.begin() + (cursor - 1));

                        cursor--;
                        if (cursor > 0)
                        {
                            curChar = buffer.at(cursor - 1);
                        }
                    }
                }
                // Erase back single character.
                else
                {
                    if (cursor > 0)
                    {
                        buffer.erase(buffer.begin() + (cursor - 1));
                        cursor--;
                    }
                }
            }

            return true;
        }
        else if (delAction.IsHeld())
        {
            if (!buffer.empty() && delAction.IsPulsed(PULSE_DELAY_SEC, PULSE_INITIAL_DELAY_SEC))
            {
                // Erase forward to end.
                if (shiftAction.IsHeld() && ctrlAction.IsHeld())
                {
                    buffer.erase(cursor, buffer.size() - cursor);
                }
                // Erase forward to next space.
                else if (ctrlAction.IsHeld())
                {
                    char curChar = '.';
                    while (cursor < buffer.size() && curChar != ' ')
                    {
                        buffer.erase(buffer.begin() + cursor);

                        cursor++;
                        if (cursor < buffer.size())
                        {
                            curChar = buffer.at(cursor);
                        }
                    }
                }
                // Erase forward single character.
                else
                {
                    if (cursor < buffer.size())
                    {
                        buffer.erase(buffer.begin() + cursor);
                        cursor++;
                    }
                }
            }

            return true;
        }

        return false;
    }

    bool TextManager::HandleCharacterAdd(const std::string& bufferId, const std::unordered_map<ActionId, Action>& actions)
    {
        auto& buffer        = _buffers[bufferId];
        uint& cursor        = _cursors[bufferId];
        auto& prevActionIds = _prevActionIds[bufferId];

        const auto& shiftAction = actions.at(In::Shift);

        bool hasNewChar = false;
        for (auto actionId : PRINTABLE_ACTION_IDS)
        {
            const auto& action = actions.at(actionId);
            const auto& chars  = PRINTABLE_ACTION_CHARS.at(actionId);

            if (!hasNewChar && action.IsHeld())
            {
                // New action.
                if (!Contains(ToSpan(prevActionIds), actionId))
                {
                    // Add character.
                    if (action.IsClicked())
                    {
                        buffer.insert(buffer.begin() + cursor, shiftAction.IsHeld() ? chars.second : chars.first);
                        cursor++;
                    }

                    prevActionIds.push_back(actionId);
                    hasNewChar = true;
                }
                // Held action.
                else if (!prevActionIds.empty() && prevActionIds.back() == actionId)
                {
                    // Add character.
                    if (action.IsPulsed(PULSE_DELAY_SEC, PULSE_INITIAL_DELAY_SEC))
                    {
                        buffer.insert(buffer.begin() + cursor, shiftAction.IsHeld() ? chars.second : chars.first);
                        cursor++;
                        hasNewChar = true;
                    }
                }
            }
            else if (action.IsReleased())
            {
                for (int i = 0; i < prevActionIds.size(); i++)
                {
                    auto prevActionId = prevActionIds[i];
                    if (prevActionId == actionId)
                    {
                        prevActionIds.erase(prevActionIds.begin() + i);
                        break;
                    }
                }
            }
        }

        return hasNewChar;
    }
}
