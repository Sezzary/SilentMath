#pragma once

namespace Silent::Renderer
{
    class View
    {
    public:
        // Fields

        Vector3 Position  = Vector3::Zero;
        Vector3 Direction = Vector3::Zero;
        Vector3 Up        = Vector3::Zero;

        Vector2 Size = Vector2::Zero;  
        float   Fov  = 0.0f;

        // Constructors

        View() = default;
    };
}
