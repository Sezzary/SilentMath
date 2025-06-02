#include "Framework.h"
#include "Engine/Renderer/Backends/OpenGl/Shader.h"

#include "Engine/Renderer/Backends/OpenGl/ElementArrayBuffer.h"
#include "Engine/Renderer/Backends/OpenGl/VertexArray.h"
#include "Engine/Renderer/Backends/OpenGl/VertexBuffer.h"

namespace Silent::Renderer
{
    void ShaderManager::Initialize(const std::string& shaderName)
    {
        // Read `vertFile` + `fragFile` and store strings.
        auto vertCode = GetFileContents("Shaders/" + shaderName + ".vert");
        auto fragCode = GetFileContents("Shaders/" + shaderName + ".frag");

        const char* vertSrc = vertCode.c_str();
        const char* fragSrc = fragCode.c_str();

        // Compile vertex shader.
        auto vertShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertShader, 1, &vertSrc, nullptr);
        glCompileShader(vertShader);
        LogError(vertShader, "VERTEX");

        // Compile fragment shader.
        auto fragShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragShader, 1, &fragSrc, nullptr);
        glCompileShader(fragShader);
        LogError(vertShader, "FRAGMENT");

        _shaderIds[shaderName] = glCreateProgram();
        auto id = _shaderIds.at(shaderName);

        glAttachShader(id, vertShader);
        glAttachShader(id, fragShader);
        glLinkProgram(id);
        //LogError(id, "PROGRAM"); // TODO: Program error logging.

        glDeleteShader(vertShader);
        glDeleteShader(fragShader);
    }

    void ShaderManager::Activate()
    {
        for (auto [keyName, id] : _shaderIds)
        {
            glUseProgram(id);
        }
    }

    void ShaderManager::Delete()
    {
        for (auto [keyName, id] : _shaderIds)
        {
            glDeleteProgram(id);
        }
    }

    std::string ShaderManager::GetFileContents(const std::string& filename)
    {
        auto input = std::ifstream(filename, std::ios::binary);
        if (input)
        {
            auto contents = std::string();
            input.seekg(0, std::ios::end);
            contents.resize(input.tellg());
            input.seekg(0, std::ios::beg);
            input.read(&contents[0], contents.size());
            input.close();
            return contents;
        }

        throw std::runtime_error("Failed to open shader file '" + filename + "`.");
    }

    void ShaderManager::LogError(uint shader, const std::string& type)
    {
        constexpr uint MSG_BUFFER_SIZE = 512;

        GLint  success = 0;
        GLchar msgBuffer[MSG_BUFFER_SIZE];

        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, MSG_BUFFER_SIZE, NULL, msgBuffer);
            Log("Failed to compile `" + type + "` shader: " + msgBuffer);
        }
    }
}
