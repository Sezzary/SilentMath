#include "Framework.h"
#include "Engine/Renderer/Backends/OpenGl/Shaders.h"

#include "Engine/Renderer/Backends/OpenGl/ElementArrayBuffer.h"
#include "Engine/Renderer/Backends/OpenGl/VertexArray.h"
#include "Engine/Renderer/Backends/OpenGl/VertexBuffer.h"

namespace Silent::Renderer
{
    void ShaderManager::Initialize(const std::string& shaderName)
    {
        constexpr char VERT_FILE_EXT[] = ".vert";
        constexpr char FRAG_FILE_EXT[] = ".frag";

        // Read source files as strings.
        auto vertCode = GetFileContents("Shaders/" + shaderName + VERT_FILE_EXT);
        auto fragCode = GetFileContents("Shaders/" + shaderName + FRAG_FILE_EXT);

        const char* vertSrc = vertCode.c_str();
        const char* fragSrc = fragCode.c_str();

        // Compile vertex shader.
        uint vertShaderId = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertShaderId, 1, &vertSrc, nullptr);
        glCompileShader(vertShaderId);
        LogError(vertShaderId, "VERTEX");

        // Compile fragment shader.
        uint fragShaderId = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragShaderId, 1, &fragSrc, nullptr);
        glCompileShader(fragShaderId);
        LogError(vertShaderId, "FRAGMENT");

        // Register shader.
        _shaderIds[shaderName] = glCreateProgram();
        uint shaderId = _shaderIds.at(shaderName);

        glAttachShader(shaderId, vertShaderId);
        glAttachShader(shaderId, fragShaderId);
        glLinkProgram(shaderId);
        //LogError(id, "PROGRAM"); // TODO: Program error logging.

        glDeleteShader(vertShaderId);
        glDeleteShader(fragShaderId);
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
        if (!input)
        {
            throw std::runtime_error("Failed to open shader file '" + filename + "`.");
        }

        auto contents = std::string();
        input.seekg(0, std::ios::end);
        contents.resize(input.tellg());
        input.seekg(0, std::ios::beg);
        input.read(&contents[0], contents.size());
        input.close();
        return contents;
    }

    void ShaderManager::LogError(uint shader, const std::string& type)
    {
        constexpr uint MSG_BUFFER_SIZE = 512;

        int  success = 0;
        char msgBuffer[MSG_BUFFER_SIZE];

        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, MSG_BUFFER_SIZE, nullptr, msgBuffer);
            Log("Failed to compile `" + type + "` shader: " + msgBuffer, LogLevel::Error);
        }
    }
}
