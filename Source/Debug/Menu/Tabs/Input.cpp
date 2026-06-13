#include "Framework.h"
#include "Debug/Menu/Tabs/Input.h"

#include "Application.h"
#include "Input/Input.h"
#include "Utils/Bitfield.h"

using namespace Silent::Input;
using namespace Silent::Utils;

namespace Silent::Debug
{
    void AddInputTab()
    {
        const auto& input = g_App.GetInput();

        if (ImGui::BeginTabItem("Input"))
        {
            g_Work.Page = Page::Input;

            // `Status` section.
            ImGui::SeparatorText("Status");
            {
                if (ImGui::BeginTable("Status", 2))
                {
                    // `Gamepad` enabled/disabled info.
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Gamepad:", 0, 0);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text(input.IsGamepadConnected() ? "Connected" : "Not connected", 0, 1);

                    // `Cursor` info.
                    const auto& cursorPos = input.GetCursorPosition();
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Cursor:", 1, 0);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("(%.2f, %.2f)", cursorPos.x, cursorPos.y, 1, 1);

                    ImGui::EndTable();
                }
            }

            // `Actions` section.
            /*ImGui::SeparatorText("Actions");
            {
                const auto& input = g_App.GetInput();

                // Collect action statuses.
                int  flagCount           = (int)ACTION_ID_GROUPS.at(USER_ACTION_GROUP_IDS.back()).back();
                auto clickedActionFlags  = Bitfield(flagCount);
                auto heldActionFlags     = Bitfield(flagCount);
                auto releasedActionFlags = Bitfield(flagCount);
                for (int i = 0; i < flagCount; i++)
                {
                    auto actionId = (ActionId)i;
                    const auto& action = input.GetAction(actionId);

                    if (action.IsClicked())
                    {
                        clickedActionFlags.Set(i);
                    }
                    if (action.IsHeld())
                    {
                        heldActionFlags.Set(i);
                    }
                    if (action.IsReleased())
                    {
                        releasedActionFlags.Set(i);
                    }
                }

                // @todo Using `c_str` crashes.

                // `Clicked` info.
                /*ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Clicked:", 0, 0);
                ImGui::TableSetColumnIndex(1);
                ImGui::Text(clickedActionFlags.ToString().c_str(), 0, 1);*/

                // `Held` info.
                /*ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Held:", 1, 0);
                ImGui::TableSetColumnIndex(1);
                ImGui::Text(heldActionFlags.ToString().c_str(), 1, 1);*/

                // `Released` info.
                /*ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Released:", 2, 0);
                ImGui::TableSetColumnIndex(1);
                ImGui::Text(releasedActionFlags.ToString().c_str(), 2, 1);*/
            //}*/

            // `Analog Axes` section.
            ImGui::SeparatorText("Analog Axes");
            {
                if (ImGui::BeginTable("Analog Axes", 2))
                {
                    // `Move axis` info.
                    const auto& moveAxis = input.GetAnalogAxis(AnalogAxisId::Move);
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Move:", 0, 0);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("(%.2f, %.2f)", moveAxis.x, moveAxis.y, 0, 1);

                    // `Camera axis` info.
                    const auto& camAxis = input.GetAnalogAxis(AnalogAxisId::Camera);
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Camera:", 1, 0);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("(%.2f, %.2f)", camAxis.x, camAxis.y, 1, 1);

                    // `Mouse axis` info.
                    const auto& mouseAxis = input.GetAnalogAxis(AnalogAxisId::Mouse);
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Mouse:", 2, 0);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("(%.2f, %.2f)", mouseAxis.x, mouseAxis.y, 2, 1);

                    // `Left stick axis` info.
                    const auto& leftStickAxis = input.GetAnalogAxis(AnalogAxisId::StickLeft);
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Left stick:", 3, 0);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("(%.2f, %.2f)", leftStickAxis.x, leftStickAxis.y, 3, 1);

                    // `Right stick axis` info.
                    const auto& rightStickAxis = input.GetAnalogAxis(AnalogAxisId::StickRight);
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Right stick:", 4, 0);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("(%.2f, %.2f)", rightStickAxis.x, rightStickAxis.y, 4, 1);

                    ImGui::EndTable();
                }
            }

            ImGui::EndTabItem();
        }
    }
}
