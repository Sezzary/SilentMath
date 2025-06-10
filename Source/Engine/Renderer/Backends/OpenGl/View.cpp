#include "Framework.h"
#include "Engine/Renderer/Backends/OpenGl/View.h"

#include "Engine/Application.h"
#include "Engine/Renderer/Backends/OpenGl/ShaderProgram.h"
#include "Engine/Input/Input.h"

using namespace Silent::Input;

namespace Silent::Renderer
{
    View::View(const Vector3& pos, const Vector2i& size)
    {
        Position = pos;
        Size     = size;
    }

    void View::ExportMatrix(float fov, float aspect, float nearPlane, float farPlane, ShaderProgram& shaderProg, const std::string& uniName)
    {
        auto viewMat = Matrix::CreateLookAt(Position, Position + Direction * 3, Up);
        auto projMat = Matrix::CreatePerspective(fov, aspect, nearPlane, farPlane);
        shaderProg.SetMatrix(uniName, projMat * viewMat);
    }

    void View::Move()
    {
        const auto& input = g_App.GetInput();

        // Move on 2D plane.
        if (input.GetAction(In::W).IsHeld())
        {
            Position.Translate(Direction, Speed);
        }
        if (input.GetAction(In::A).IsHeld())
        {
            Position += Speed * -Vector3::Normalize(Vector3::Cross(Direction, Up));
        }
        if (input.GetAction(In::S).IsHeld())
        {
            Position.Translate(Direction, -Speed);
        }
        if (input.GetAction(In::D).IsHeld())
        {
            Position += Speed * glm::normalize(glm::cross(Direction, Up));
        }

        // Move vertically.
        if (input.GetAction(In::Space).IsHeld())
        {
            Position.Translate(Up, Speed);
        }
        if (input.GetAction(In::Ctrl).IsHeld())
        {
            Position.Translate(Up, -Speed);
        }

        // Modulate speed.
        if (input.GetAction(In::Shift).IsHeld())
        {
            Speed = 0.1f;
        }
        else
        {
            Speed = 0.05f;
        }

        const auto& axis = input.GetAnalogAxis(AnalogAxisId::Mouse);
        // TODO: Rotation with mouse.
    }
}
