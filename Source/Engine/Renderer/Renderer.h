#pragma once

namespace Silent::Renderer
{
    class Renderer
    {
    public:
        // Constructors

        Renderer() = default;

        // Utilities

        void Update();
    };

    extern Renderer g_Renderer;
}
