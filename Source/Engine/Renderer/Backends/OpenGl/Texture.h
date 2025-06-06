#pragma once

namespace Silent::Renderer
{
    class ShaderProgram;

    class Texture
    {
    private:
        // Fields

        uint _id = 0;

    public:
        // Constructors

        Texture() = default;
        Texture(const std::string& filename, GLenum slot, GLenum format, GLenum pixelType);

        // Utilities

        void Bind();
        void Unbind();
        void Delete();

        void TextureUnit(ShaderProgram& shaderProg, const std::string& uniName, uint unitId);
        void RefreshFiltering();
    };
}
