#include "Framework.h"
#include "Engine/Renderer/Backends/OpenGl/Shaders.h"

#include "Engine/Renderer/Backends/OpenGl/ElementArrayBuffer.h"
#include "Engine/Renderer/Backends/OpenGl/VertexArray.h"
#include "Engine/Renderer/Backends/OpenGl/VertexBuffer.h"

namespace Silent::Renderer
{
    uint ShaderProgramManager::GetProgramId(const std::string& programName) const
    {
        auto it = _programIds.find(programName);
        if (it == _programIds.end())
        {
            Log("Attempted to get ID for missing shader program '" + programName + "'.", LogLevel::Warning);
            return 0;
        }

        auto [keyName, id] = *it;
        return id;
    }

    void ShaderProgramManager::Initialize(const std::string& shaderName)
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
        LogShaderError(vertShaderId, "VERTEX");

        // Compile fragment shader.
        uint fragShaderId = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragShaderId, 1, &fragSrc, nullptr);
        glCompileShader(fragShaderId);
        LogShaderError(vertShaderId, "FRAGMENT");

        // Register shader program.
        _programIds[shaderName] = glCreateProgram();
        uint programId = _programIds.at(shaderName);

        // Create shader program.
        glAttachShader(programId, vertShaderId);
        glAttachShader(programId, fragShaderId);
        glLinkProgram(programId);
        LogProgramError(programId);

        // Cleanup.
        glDeleteShader(vertShaderId);
        glDeleteShader(fragShaderId);
    }

    void ShaderProgramManager::Activate(const std::string& programName)
    {
        auto it = _programIds.find(programName);
        if (it == _programIds.end())
        {
            Log("Attempted to activate missing shader program '" + programName + "'.", LogLevel::Warning);
            return;
        }

        auto [keyName, id] = *it;
        glUseProgram(id);
    }

    void ShaderProgramManager::Delete()
    {
        for (auto [keyName, id] : _programIds)
        {
            glDeleteProgram(id);
        }
    }

    std::string ShaderProgramManager::GetFileContents(const std::string& filename)
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

    void ShaderProgramManager::LogShaderError(uint shaderId, const std::string& type)
    {
        constexpr uint MSG_BUFFER_SIZE = 512;

        int  success = 0;
        char msgBuffer[MSG_BUFFER_SIZE];

        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shaderId, MSG_BUFFER_SIZE, nullptr, msgBuffer);
            Log("Failed to compile `" + type + "` shader: " + msgBuffer, LogLevel::Error);
        }
    }

    void ShaderProgramManager::LogProgramError(uint programId)
    {
        constexpr uint MSG_BUFFER_SIZE = 512;

        int  success = 0;
        char msgBuffer[MSG_BUFFER_SIZE];

        glGetProgramiv(programId, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(programId, MSG_BUFFER_SIZE, nullptr, msgBuffer);
            Log("Failed to create '" + std::to_string(programId) + "' shader program:" + msgBuffer, LogLevel::Error);
        }
    }
}
