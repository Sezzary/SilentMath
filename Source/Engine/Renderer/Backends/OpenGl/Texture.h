#pragma once

namespace Silent::Renderer
{
    class ShaderProgram;

    class Texture
    {
    private:
        // Fields

        uint _id   = 0;
        uint _unit = 0;

    public:
        // Constructors

        Texture() = default;
        Texture(const std::filesystem::path& filename, GLenum unit, GLenum format, GLenum pixelType);

        // Utilities

        void Bind();
        void Unbind();
        void Delete();

        void SetTextureUnit(ShaderProgram& shaderProg, const std::string& uniName, uint unitId);
        void RefreshFilter();

    private:
        // Helpers

        void SetNearestFilter();
        void SetBilinearFilter();
    };
}
