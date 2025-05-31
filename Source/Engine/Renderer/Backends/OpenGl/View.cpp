#include "Framework.h"
#include "Engine/Renderer/Backends/OpenGl/View.h"

#include "Engine/Renderer/Backends/OpenGl/Shader.h"

namespace Silent::Renderer
{
    View::View(const Vector3& pos, const Vector2i& size)
    {
        Position = pos;
        Size     = size;
    }

    void View::ExportMatrix(float fov, float nearPlane, float farPlane, ShaderManager& shader, const std::string& uni)
    {
        //auto viewMat  = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, -2.0f));
        auto viewMat = glm::lookAt(Position, Position + Direction, Up);
        auto projMat = glm::perspective(fov, (float)Size.x / (float)Size.y, nearPlane, farPlane);

        glUniformMatrix4fv(glGetUniformLocation(shader.Id, uni.c_str()), 1, GL_FALSE, glm::value_ptr(projMat * viewMat));
    }
}
