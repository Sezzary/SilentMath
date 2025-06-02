#pragma once

namespace Silent::Renderer
{
    class ShaderManager
    {
    public:
        // Fields

        std::unordered_map<std::string, GLuint> _shaderIds = {};

        // Constructors

        ShaderManager() = default;

        // Utilities

        void Initialize(const std::string& shaderName);
        void Activate();
        void Delete();

    private:
        // Helpers

        std::string GetFileContents(const std::string& filename);
        void        LogError(uint shader, const std::string& type);
    };
}
