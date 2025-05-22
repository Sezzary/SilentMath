#pragma once

namespace Silent::Hud
{
    struct Toast
    {
        std::string Message  = {};
        Vector2     Position = Vector2::Zero;
        Color       Col      = Color::Black;
        int         Life     = 0;
    };

    class ToastManager
    {
    private:
        // Constants

        static constexpr uint  TOAST_COUNT_MAX       = 8;
        static constexpr float LIFE_SEC_MAX          = 4.0f;
        static constexpr float LIFE_SEC_START_FADING = LIFE_SEC_MAX * 0.25f;

        // Fileds

        std::vector<Toast> _toasts = {};

    public:
        // Constructors

        ToastManager() = default;

        // Utilities

        void AddToast(const std::string& msg, const Color& color = Color::White);
        void Update();
        void Render();

    private:
        // Helpers

        void ClearInactiveToasts();
    };
}
