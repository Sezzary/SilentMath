#include "Framework.h"
#include "Engine/Renderer/Backends/OpenGl/Texture.h"

#include "Engine/Renderer/Backends/OpenGl/Shaders.h"

namespace Silent::Renderer
{
    Texture::Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType)
    {
        // Assign type.
        _type = texType;

        auto res               = Vector2i::Zero;
        int  colorChannelCount = 0;
        
        // Read image from file.
        stbi_set_flip_vertically_on_load(true);
        uchar* bytes = stbi_load(image, &res.x, &res.y, &colorChannelCount, 0);

        // Generate OpenGL texture object.
        glGenTextures(1, &_id);

        // Assign texture to texture unit.
        glActiveTexture(slot);
        glBindTexture(texType, _id);

        // Configure algorithm type used to make image smaller or bigger.
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
        glBindTexture(_type, _id);
    }

    void Texture::Unbind()
    {
        glBindTexture(_type, 0);
    }

    void Texture::Delete()
    {
        glDeleteTextures(1, &_id);
    }

    void Texture::TextureUnit(ShaderProgramManager& shaders, const std::string& uniform, uint unit)
    {
        // Get uniform location.
        uint texUniLoc = glGetUniformLocation(shaders.GetProgramId("Default"), uniform.c_str());

        // Set uniform value.
        shaders.Activate("Default");
        glUniform1i(texUniLoc, unit);
    }
}
