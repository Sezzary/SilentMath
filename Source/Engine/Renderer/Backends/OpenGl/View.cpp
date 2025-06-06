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

    void View::ExportMatrix(float fov, float nearPlane, float farPlane, ShaderProgram& shaderProgram, const std::string& uni)
    {
        //auto viewMat  = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, -2.0f));
        auto viewMat = glm::lookAt(Position, Position + Direction, Up);
        auto projMat = glm::perspective(fov, (float)Size.x / (float)Size.y, nearPlane, farPlane);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.GetId(), uni.c_str()), 1, GL_FALSE, glm::value_ptr(projMat * viewMat));
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
            Position += Speed * -glm::normalize(glm::cross(Direction, Up));
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
