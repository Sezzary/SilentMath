#include "Framework.h"
#include "Engine/Renderer/Backends/OpenGl/ShaderProgram.h"

#include "Engine/Renderer/Backends/OpenGl/ElementBuffer.h"
#include "Engine/Renderer/Backends/OpenGl/VertexArray.h"
#include "Engine/Renderer/Backends/OpenGl/VertexBuffer.h"

namespace Silent::Renderer
{
    ShaderProgram::ShaderProgram(const std::string& name)
    {
        constexpr char VERT_FILE_EXT[] = ".vert";
        constexpr char FRAG_FILE_EXT[] = ".frag";

        // Set name.
        _name = name;

        // Read source files as strings.
        auto vertCode = GetFileContents("Shaders/" + name + VERT_FILE_EXT);
        auto fragCode = GetFileContents("Shaders/" + name + FRAG_FILE_EXT);

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

        // Create shader program.
        _id = glCreateProgram();
        glAttachShader(_id, vertShaderId);
        glAttachShader(_id, fragShaderId);
        glLinkProgram(_id);
        LogProgramError(_id);

        // Cleanup.
        glDeleteShader(vertShaderId);
        glDeleteShader(fragShaderId);
    }

    uint ShaderProgram::GetId() const
    {
        return _id;
    }

    const std::string& ShaderProgram::GetName() const
    {
        return _name;
    }

    void ShaderProgram::SetBool(const std::string& uniName, bool val) const
    {
        glUniform1i(glGetUniformLocation(_id, uniName.c_str()), (int)val);
    }

    void ShaderProgram::SetInt(const std::string& uniName, int val) const
    {
        glUniform1i(glGetUniformLocation(_id, uniName.c_str()), val);
    }

    void ShaderProgram::SetUint(const std::string& uniName, uint val) const
    {
        glUniform1ui(glGetUniformLocation(_id, uniName.c_str()), val);
    }

    void ShaderProgram::SetFloat(const std::string& uniName, float val) const
    {
        glUniform1f(glGetUniformLocation(_id, uniName.c_str()), val);
    }

    void ShaderProgram::SetVec2(const std::string& uniName, const Vector2& val) const
    {
        glUniform2fv(glGetUniformLocation(_id, uniName.c_str()), 1, &val[0]);
    }

    void ShaderProgram::SetVec3(const std::string& uniName, const Vector3& val) const
    {
        glUniform3fv(glGetUniformLocation(_id, uniName.c_str()), 1, &val[0]);
    }

    void ShaderProgram::SetVec4(const std::string& uniName, const Vector4& val) const
    {
        glUniform4fv(glGetUniformLocation(_id, uniName.c_str()), 1, &val[0]);
    }

    void ShaderProgram::SetMatrix(const std::string& uniName, const Matrix& val) const
    {
        glUniformMatrix4fv(glGetUniformLocation(_id, uniName.c_str()), 1, GL_FALSE, &val[0][0]);
    }

    void ShaderProgram::SetIntArray(const std::string& uniName, const std::span<int>& val) const
    {
        glUniform1iv(glGetUniformLocation(_id, uniName.c_str()), val.size(), val.data());
    }

    void ShaderProgram::SetUintArray(const std::string& uniName, const std::span<uint>& val) const
    {
        glUniform1uiv(glGetUniformLocation(_id, uniName.c_str()), val.size(), val.data());
    }

    void ShaderProgram::SetFloatArray(const std::string& uniName, const std::span<float>& val) const
    {
        glUniform1fv(glGetUniformLocation(_id, uniName.c_str()), val.size(), val.data());
    }

    void ShaderProgram::SetVec2Array(const std::string& uniName, const std::span<Vector2>& val) const
    {
        glUniform2fv(glGetUniformLocation(_id, uniName.c_str()), val.size(), (const float*)val.data());
    }

    void ShaderProgram::SetVec3Array(const std::string& uniName, const std::span<Vector3>& val) const
    {
        glUniform3fv(glGetUniformLocation(_id, uniName.c_str()), val.size(), (const float*)val.data());
    }

    void ShaderProgram::SetVec4Array(const std::string& uniName, const std::span<Vector4>& val) const
    {
        glUniform4fv(glGetUniformLocation(_id, uniName.c_str()), val.size(), (const float*)val.data());
    }

    void ShaderProgram::SetMatrixArray(const std::string& uniName, const std::span<Matrix>& val) const
    {
        glUniformMatrix4fv(glGetUniformLocation(_id, uniName.c_str()), 1, GL_FALSE, (const float*)val.data());
    }

    void ShaderProgram::Activate()
    {
        glUseProgram(_id);
    }

    void ShaderProgram::Delete()
    {
        glDeleteProgram(_id);
    }

    std::string ShaderProgram::GetFileContents(const std::string& filename)
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

    void ShaderProgram::LogShaderError(uint shaderId, const std::string& type)
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

    void ShaderProgram::LogProgramError(uint programId)
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
