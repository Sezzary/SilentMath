#include "Framework.h"
#include "Engine/Renderer/Backends/OpenGl/Texture.h"

#include "Engine/Application.h"
#include "Engine/Renderer/Backends/OpenGl/ShaderProgram.h"
#include "Engine/Services/Assets/Assets.h"
#include "Engine/Services/Assets/Parsers/Tim.h" // TODO: Shouldn't need this.
#include "Engine/Services/Options.h"

using namespace Silent::Assets;
using namespace Silent::Services;

namespace Silent::Renderer
{
    Texture::Texture(const std::filesystem::path& filename, GLenum unit, GLenum format, GLenum pixelType)
    {
        const auto& options = g_App.GetOptions();

        // Store texture unit.
        _unit = unit;

        // Generate texture object.
        glGenTextures(1, &_id);
        glBindTexture(GL_TEXTURE_2D, _id);

        // Configure texture repetition type.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Configure algorithm type used to make image smaller or bigger.
        switch (options->TextureFilter)
        {
            default:
            case TextureFilterType::Nearest:
            {
                SetNearestFilter();
                break;
            }

            case TextureFilterType::Bilinear:
            {
                SetBilinearFilter();
                break;
            }
        }

        // HACK!!!! Demonstrating TIM file load.
        // Read image from file.
        if (filename.extension().string() == ".TIM")
        {
            auto& assets = g_App.GetAssets();

            // Load asset.
            auto name = "TIM\\" + filename.filename().string();
            assets.LoadAsset(name).wait();

            // Get asset data.
            const auto& asset = assets.GetAsset(name);
            auto data         = GetAssetData<TimAsset>(asset->Data);

            // Assign image to texture object.
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data->Resolution.x, data->Resolution.y, 0, format, pixelType, data->Pixels.data());
            glGenerateMipmap(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        else
        {
            stbi_set_flip_vertically_on_load(true);
            auto   res               = Vector2i::Zero;
            int    colorChannelCount = 0;
            uchar* data              = stbi_load(filename.string().c_str(), &res.x, &res.y, &colorChannelCount, 0);

            // Assign image to texture object.
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, res.x, res.y, 0, format, pixelType, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);

            // Cleanup.
            stbi_image_free(data);
        }
    }

    void Texture::Bind()
    {
        glActiveTexture(_unit);
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

    void Texture::SetTextureUnit(ShaderProgram& shaderProg, const std::string& uniName, uint unit)
    {
        // Get uniform location.
        uint texUniLoc = glGetUniformLocation(shaderProg.GetId(), uniName.c_str());

        // Set uniform value.
        shaderProg.Activate();
        glUniform1i(texUniLoc, unit);
    }

    void Texture::RefreshFilter()
    {
        const auto& options = g_App.GetOptions();

        Bind();
        switch (options->TextureFilter)
        {
            default:
            case TextureFilterType::Nearest:
            {
                SetNearestFilter();
                break;
            }

            case TextureFilterType::Bilinear:
            {
                SetBilinearFilter();
                break;
            }
        }
        Unbind();
    }

    void Texture::SetNearestFilter()
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
    }

    void Texture::SetBilinearFilter()
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        float anisotropyMax = 0.0f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisotropyMax);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropyMax);
    }
}
