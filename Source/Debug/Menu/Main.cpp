#include "Framework.h"
#include "Debug/Menu/Main.h"

#include "Debug/Debug.h"
#include "Debug/GameData.h"
#include "Debug/Menu/Tabs/Cheats.h"
#include "Debug/Menu/Tabs/General.h"
#include "Debug/Menu/Tabs/Input.h"
#include "Debug/Menu/Tabs/Options.h"
#include "Debug/Menu/Tabs/Renderer.h"
#include "Debug/Menu/Tabs/Resources.h"

namespace Silent::Debug
{
    void CreateMenu()
    {
        CreateGui([]()
        {
            ImGui::Begin("Debug Menu");

            // Tabs section.
            if (ImGui::BeginTabBar("MainTabs", ImGuiTabBarFlags_FittingPolicyScroll))
            {
                AddGeneralTab();
                AddRendererTab();
                AddResourcesTab();
                AddInputTab();
                AddCheatsTab();
                AddOptionsTab();

                ImGui::EndTabBar();
            }

            // `Messages` section.
            ImGui::SeparatorText("Messages");
            {
                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));

                if (ImGui::BeginChild("MessageBox",
                                      ImVec2(-FLT_MIN, ImGui::GetTextLineHeightWithSpacing() * 8),
                                      ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeY))
                {
                    for (const auto& msg : g_Work.PrevMessages)
                    {
                        ImGui::Bullet();
                        ImGui::TextWrapped(msg.c_str());
                    }
                }

                ImGui::PopStyleColor();
                ImGui::EndChild();
            }

            ImGui::End();
        });
    }
}
