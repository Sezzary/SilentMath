#pragma once

namespace Silent::Renderer
{
    class ShaderManager;

    class View
    {
    public:
        // Fields

        Vector3 Position  = Vector3::Zero;
        Vector3 Direction = Vector3(0.0f, 0.0f, -1.0f);
        Vector3 Up        = Vector3::UnitY;

        Vector2i Size = Vector2i::Zero;  
        float    Fov  = 0.0f;

        float Velocity    = 0.1f;
        float Sensitivity = 100.0f;

        // Constructors

        View() = default;
        View(const Vector3& pos, const Vector2i& size);

        // Utilities

        void ExportMatrix(float fov, float nearPlane, float farPlane, ShaderManager& shader, const std::string& uni);
    };
}
