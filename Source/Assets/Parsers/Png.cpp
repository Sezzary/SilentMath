#include "Framework.h"
#include "Assets/Parsers/Png.h"

#include "Application.h"
#include "Renderer/Common/Constants.h"
#include "Services/Filesystem.h"

using namespace Silent::Renderer;
using namespace Silent::Services;

namespace Silent::Assets
{
    std::shared_ptr<void> ParsePng(const std::filesystem::path& filename)
    {
        const auto& fs = g_App.GetFilesystem();

        // Create asset.
        auto asset = PngAsset{};

        // Read pixels.
        int    channels = 0;
        uchar* pixels   = stbi_load(filename.c_str(), &asset.Resolution.x, &asset.Resolution.y, &channels, RGBA_COMP_COUNT);
        if (pixels == nullptr) 
        {
            throw std::runtime_error(Fmt("Failed to open PNG `{}`.",
                                         std::filesystem::relative(filename, fs.GetAssetsDirectory()).string()));
        }

        // Copy pixels.
        asset.Pixels = std::vector<byte>(pixels, pixels + ((asset.Resolution.x * asset.Resolution.y) * RGBA_COMP_COUNT));

        // Free resources.
        stbi_image_free(pixels);

        return std::make_shared<PngAsset>(std::move(asset));
    }
}
