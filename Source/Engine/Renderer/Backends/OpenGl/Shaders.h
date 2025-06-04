#pragma once

namespace Silent::Renderer
{
    class ShaderManager
    {
    public:
        // Fields

        std::unordered_map<std::string, uint> _programIds = {}; // Key = program name, value = program ID.

        // Constructors

        ShaderManager() = default;

        // Utilities

        void Initialize(const std::string& programName);
        void Activate(const std::string& programName);
        void Delete();

    private:
        // Helpers

        std::string GetFileContents(const std::string& filename);
        void        LogShaderError(uint shaderId, const std::string& type);
        void        LogProgramError(uint programId);
    };
}
