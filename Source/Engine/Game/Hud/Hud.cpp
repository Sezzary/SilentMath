#include "Framework.h"
#include "Engine/Game/Hud/Hud.h"

#include "Engine/Game/Hud/Toast.h"

namespace Silent::Hud
{
    void HudManager::Update()
    {
        _toasts.Update();
    }

    void HudManager::Render()
    {
        _toasts.Render();
    }
}
