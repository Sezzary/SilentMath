#include "Framework.h"
#include "Engine/Renderer/Backends/OpenGl/Texture.h"

#include "Engine/Application.h"
#include "Engine/Renderer/Backends/OpenGl/ShaderProgram.h"
#include "Engine/Services/Configuration.h"

using namespace Silent::Services;

namespace Silent::Renderer
{
    Texture::Texture(const std::string& filename, GLenum slot, GLenum format, GLenum pixelType)
    {
        const auto& options = g_App.GetConfig().GetOptions();

        // Configure texture repetition type.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Configure algorithm type used to make image smaller or bigger.
        switch (options.TextureFilterType)
        {
            default:
            case TextureFilterType::Nearest:
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                break;
            }

            case TextureFilterType::Bilinear:
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
        }

        auto res               = Vector2i::Zero;
        int  colorChannelCount = 0;
        
        // Read image from file.
        stbi_set_flip_vertically_on_load(true);
        uchar* data = stbi_load(filename.c_str(), &res.x, &res.y, &colorChannelCount, 0);

        // Generate texture object.
        glGenTextures(1, &_id);

        // Assign image to texture object.
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, res.x, res.y, 0, format, pixelType, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Assign texture to texture unit.
        glActiveTexture(slot);
        glBindTexture(GL_TEXTURE_2D, _id);

        // Cleanup.
        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::Bind()
    {
        glBindTexture(GL_TEXTURE_2D, _id);
    }

    void Texture::Unbind()
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::Delete()
    {
        glDeleteTextures(1, &_id);
    }

    void Texture::TextureUnit(ShaderProgram& shaderProg, const std::string& uniName, uint unit)
    {
        // Get uniform location.
        uint texUniLoc = glGetUniformLocation(shaderProg.GetId(), uniName.c_str());

        // Set uniform value.
        shaderProg.Activate();
        glUniform1i(texUniLoc, unit);
    }

    void Texture::RefreshFiltering()
    {
        const auto& options = g_App.GetConfig().GetOptions();

        Bind();
        switch (options.TextureFilterType)
        {
            default:
            case TextureFilterType::Nearest:
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                break;
            }

            case TextureFilterType::Bilinear:
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
        }
        Unbind();
    }
}
