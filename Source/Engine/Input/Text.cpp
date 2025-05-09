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

    const std::string& TextManager::GetTextBuffer(const std::string& bufferId) const
    {
        constexpr char EMPTY[] = "";

        auto it = _buffers.find(bufferId);
        if (it == _buffers.end())
        {
            return EMPTY;
        }

        const auto& [keyId, buffer] = *it;
        return buffer;
    }

    void TextManager::UpdateTextBuffer(const std::string& bufferId, const std::unordered_map<ActionId, Action>& actions)
    {
        // Get buffer data.
        auto& buffer        = _buffers[bufferId];
        uint& cursor        = _cursors[bufferId];
        auto& prevActionIds = _prevActionIds[bufferId];

        // Move cursor.
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

            return;
        }

        // Query modifier actions.
        bool isShiftHeld = actions.at(In::Shift).IsHeld();
        bool isCtrlHeld  = actions.at(In::Ctrl).IsHeld();

        // Clear characters.
        const auto& bsAction  = actions.at(In::Backspace);
        const auto& delAction = actions.at(In::Delete);
        if (bsAction.IsHeld())
        {
            if (!buffer.empty() && bsAction.IsPulsed(PULSE_DELAY_SEC, PULSE_INITIAL_DELAY_SEC))
            {
                if (isShiftHeld && isCtrlHeld)
                {
                    ClearTextBuffer(id);
                }
                else if (isCtrlHeld)
                {
                    char currentChar = '.';
                    while (cursor > 0 && currentChar != ' ')
                    {
                        buffer.erase(buffer.begin() + (cursor - 1));

                        cursor--;
                        if (cursor > 0)
                        {
                            currentChar = buffer.at(cursor - 1);
                        }
                    }
                }
                else
                {
                    if (cursor > 0)
                    {
                        buffer.erase(buffer.begin() + (cursor - 1));
                        cursor--;
                    }
                }
            }

            return;
        }
        else if (delAction.IsHeld())
        {
            if (!buffer.empty() && delAction.IsPulsed(PULSE_DELAY_SEC, PULSE_INITIAL_DELAY_SEC))
            {
                if (isShiftHeld && isCtrlHeld)
                {
                    ClearTextBuffer(bufferId);
                }
                else if (isCtrlHeld)
                {
                    char currentChar = '.';
                    while (cursor < buffer.size() && currentChar != ' ')
                    {
                        buffer.erase(buffer.begin() + cursor);

                        cursor++;
                        if (cursor < buffer.size())
                        {
                            currentChar = buffer.at(cursor);
                        }
                    }
                }
                else
                {
                    if (cursor < buffer.size())
                    {
                        buffer.erase(buffer.begin() + cursor);
                        cursor++;
                    }
                }
            }

            return;
        }

        static auto prevActionId = std::optional<ActionId>();

        bool hasChar = false;
        for (auto actionId : PRINTABLE_ACTION_IDS)
        {
            const auto& action = actions.at(actionId);
            const auto& chars  = PRINTABLE_ACTION_CHARS.at(actionId);

            if (!hasChar && action.IsHeld())
            {
                // New character.
                if (!Contains(ToSpan(prevActionIds), actionId))
                {
                    // Append.
                    if (action.IsClicked())
                    {
                        buffer += isShiftHeld ? chars.second : chars.first;
                    }

                    prevActionIds.push_back(actionId);
                    hasChar = true;
                    break;
                }
                // Held character.
                else if (!prevActionIds.empty() && prevActionIds.back() == actionId)
                {
                    // Append.
                    if (action.IsPulsed(PULSE_DELAY_SEC, PULSE_INITIAL_DELAY_SEC))
                    {
                        buffer += isShiftHeld ? chars.second : chars.first;
                        hasChar = true;
                        break;
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
                        // TODO
                        //Erase(ToSpan(prevActionIds), i);
                        break;
                    }
                }
            }
        }

        _prevActionIds.erase(bufferId);
    }

    void TextManager::ClearTextBuffer(const std::string& bufferId)
    {
        _buffers.erase(bufferId);
        _cursors.erase(bufferId);
        _prevActionIds.erase(bufferId);
    }
}
