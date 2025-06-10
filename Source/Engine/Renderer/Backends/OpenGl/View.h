#pragma once

namespace Silent::Renderer
{
    class ShaderProgram;

    class View
    {
    public:
        // Fields

        Vector3 Position  = Vector3::Zero;
        Vector3 Direction = -Vector3::UnitZ;
        Vector3 Up        = Vector3::UnitY;

        Vector2i Size = Vector2i::Zero;  
        float    Fov  = 45.0f;

        float Speed = 0.02f;

        // Constructors

        View() = default;
        View(const Vector3& pos, const Vector2i& size);

        // Utilities

        void ExportMatrix(float fov, float aspect, float nearPlane, float farPlane, ShaderProgram& shaderProg, const std::string& uniName);
        void Move();
    };
}
