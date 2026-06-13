#include "Framework.h"
#include "Debug/Menu/Tabs/Cheats.h"

#include "Debug/Debug.h"
#include "Debug/GameData.h"

namespace Silent::Debug
{
    void AddCheatsTab()
    {
        if (ImGui::BeginTabItem("Cheats"))
        {
            g_Work.Page = Page::Cheats;

            // `Freeze mode` checkbox.
            ImGui::Checkbox("Freeze mode", &g_Work.EnableFreezeMode);

            ImGui::EndTabItem();
        }
    }
}
