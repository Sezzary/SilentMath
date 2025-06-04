#include "Framework.h"
#include "Engine/Renderer/Backends/OpenGl/Texture.h"

#include "Engine/Renderer/Backends/OpenGl/Shader.h"

namespace Silent::Renderer
{
    Texture::Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType)
    {
        // Assign type.
        Type = texType;

        auto res               = Vector2i::Zero;
        int  colorChannelCount = 0;
        
        // Reads image from file.
        stbi_set_flip_vertically_on_load(true);
        uchar* bytes = stbi_load(image, &res.x, &res.y, &colorChannelCount, 0);

        // Generates OpenGL texture object.
        glGenTextures(1, &Id);

        // Assigns texture to texture unit.
        glActiveTexture(slot);
        glBindTexture(texType, Id);

        // Configure type of algorithm used to make image smaller or bigger.
        glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Configure texture repetition.
        glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Extra lines for `GL_CLAMP_TO_BORDER`.
        // float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
        // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);

        // Assign image to OpenGL texture object.
        glTexImage2D(texType, 0, GL_RGBA, res.x, res.y, 0, format, pixelType, bytes);
        glGenerateMipmap(texType);

        // Cleanup.
        stbi_image_free(bytes);
        glBindTexture(texType, 0);
    }
    void Texture::Bind()
    {
        glBindTexture(Type, Id);
    }

    void Texture::Unbind()
    {
        glBindTexture(Type, 0);
    }

    void Texture::Delete()
    {
        glDeleteTextures(1, &Id);
    }

    void Texture::TextureUnit(ShaderManager& shader, const char* uniform, uint unit)
    {
        // Get uniform location.
        uint texUni = glGetUniformLocation(shader._shaderIds.at("Default"), uniform);

        // Set uniform value.
        shader.Activate();
        glUniform1i(texUni, unit);
    }
}
