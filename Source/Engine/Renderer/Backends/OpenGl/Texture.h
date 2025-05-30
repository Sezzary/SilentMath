#pragma once

namespace Silent::Renderer
{
    class ShaderManager;

    class Texture
    {
    public:
        // Fields

        GLuint Id   = 0;
        GLenum Type = 0;

        // Constructors

        Texture() = default;
        Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType);

        // Utilities

        void Bind();
        void Unbind();
        void Delete();

        void TextureUnit(ShaderManager& shader, const char* uniform, GLuint unitId);
    };
}
