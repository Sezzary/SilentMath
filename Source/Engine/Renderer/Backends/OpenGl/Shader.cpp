#include "Framework.h"
#include "Engine/Renderer/Backends/OpenGl/Shader.h"

#include "Engine/Renderer/Backends/OpenGl/ElementArrayBuffer.h"
#include "Engine/Renderer/Backends/OpenGl/VertexArray.h"
#include "Engine/Renderer/Backends/OpenGl/VertexBuffer.h"

namespace Silent::Renderer
{
    void ShaderManager::Initialize(const char* vertFile, const char* fragFile)
    {
        // Read vertexFile and fragmentFile and store the strings
        auto vertCode = GetFileContents(vertFile);
        auto fragCode = GetFileContents(fragFile);

        const char* vertSrc = vertCode.c_str();
        const char* fragSrc = fragCode.c_str();

        uint vertxShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertxShader, 1, &vertSrc, NULL);
        glCompileShader(vertxShader);

        uint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragShader, 1, &fragSrc, NULL);
        glCompileShader(fragShader);

        Id = glCreateProgram();
        glAttachShader(Id, vertxShader);
        glAttachShader(Id, fragShader);
        glLinkProgram(Id);

        glDeleteShader(vertxShader);
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
            input.read(&contents[0], contents.size());
            input.close();
            return contents;
        }

        Log("AAAAA");
        
        throw std::runtime_error("Failed to open file: " + std::string(filename));
    }
}
