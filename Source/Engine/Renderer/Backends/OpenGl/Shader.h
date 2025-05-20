#pragma once

namespace Silent::Renderer
{
    class ShaderManager
    {
    public:
        uint Id = 0;

        // Constructors

        ShaderManager() = default;

        // Utilities

        void Initialize(const char* vertFile, const char* fragFile);
        void Activate();
        void Delete();

    private:
        // Helpers

        std::string GetFileContents(const char* filename);
    };
}
