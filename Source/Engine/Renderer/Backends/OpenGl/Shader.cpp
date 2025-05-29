#include "Framework.h"
#include "Engine/Renderer/Backends/OpenGl/Shader.h"

#include "Engine/Renderer/Backends/OpenGl/ElementArrayBuffer.h"
#include "Engine/Renderer/Backends/OpenGl/VertexArray.h"
#include "Engine/Renderer/Backends/OpenGl/VertexBuffer.h"

namespace Silent::Renderer
{
    void ShaderManager::Initialize(const char* vertFile, const char* fragFile)
    {
        // Read vertexFile and fragmentFile and store strings.
        auto vertCode = GetFileContents(vertFile);
        auto fragCode = GetFileContents(fragFile);

        const char* vertSrc = vertCode.c_str();
        const char* fragSrc = fragCode.c_str();

        // Compile vertex shader.
        GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertShader, 1, &vertSrc, nullptr);
        glCompileShader(vertShader);
        LogError(vertShader, "VERTEX");

        // Compile fragment shader.
        GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragShader, 1, &fragSrc, nullptr);
        glCompileShader(fragShader);
        LogError(vertShader, "FRAGMENT");

        Id = glCreateProgram();
        glAttachShader(Id, vertShader);
        glAttachShader(Id, fragShader);
        glLinkProgram(Id);
        //LogError(Id, "PROGRAM"); // TODO: Program error logging.

        glDeleteShader(vertShader);
        glDeleteShader(fragShader);
    }

    void ShaderManager::Activate()
    {
        glUseProgram(Id);
    }

    void ShaderManager::Delete()
    {
        glDeleteProgram(Id);
    }

    std::string ShaderManager::GetFileContents(const char* filename)
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

        throw std::runtime_error("Failed to open shader file '" + std::string(filename) + "`.");
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
