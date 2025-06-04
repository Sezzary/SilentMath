#pragma once

namespace Silent::Renderer
{
    class ShaderProgramManager
    {
    private:
        // Fields

        std::unordered_map<std::string, uint> _programIds = {}; // Key = program name, value = program ID.

    public:
        // Constructors

        ShaderProgramManager() = default;

        // Getters

        uint GetProgramId(const std::string& programName) const;

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
