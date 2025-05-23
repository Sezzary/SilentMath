#pragma once

#include "Engine/Game/Hud/Toast.h"

namespace Silent::Hud
{
    class HudManager
    {
    private:
        // Fields

        ToastManager _toasts = ToastManager();

    public:
        // Constructors

        HudManager() = default;

        // Utilities

        void Update();
        void Render();
    };
}
