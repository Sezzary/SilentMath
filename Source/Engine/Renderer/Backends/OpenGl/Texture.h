#pragma once

namespace Silent::Renderer
{
    class ShaderManager;

    class Texture
    {
    private:
        // Fields

        uint   _id   = 0;
        GLenum _type = 0;

    public:
        // Constructors

        Texture() = default;
        Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType);

        // Utilities

        void Bind();
        void Unbind();
        void Delete();

        void TextureUnit(ShaderManager& shader, const std::string& uniform, uint unitId);
    };
}
