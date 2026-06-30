#include "Framework.h"
#include "Debug/Menu/Tabs/Resources.h"

#include "Application.h"
#include "Assets/AssetStreamer.h"

using namespace Silent::Assets;

namespace Silent::Debug
{
    void AddResourcesTab()
    {
        const auto& assets = g_App.GetAssets();

        if (ImGui::BeginTabItem("Resources"))
        {
            // `Loaded Assets` section.
            ImGui::SeparatorText("Loaded Assets");
            {
                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));
                if (ImGui::BeginChild("LoadedAssetsBox",
                                      ImVec2(-FLT_MIN, ImGui::GetTextLineHeightWithSpacing() * 8),
                                      ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeY))
                {
                    auto assetNames = assets.GetLoadedNames();
                    for (const auto& assetName : assetNames)
                    {
                        ImGui::Bullet();
                        ImGui::TextWrapped(assetName.c_str());
                    }
                }
                ImGui::PopStyleColor();
                ImGui::EndChild();
            }

            ImGui::EndTabItem();
        }
    }
}
