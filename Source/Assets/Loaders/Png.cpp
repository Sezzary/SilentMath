#include "Framework.h"
#include "Assets/Loaders/Png.h"

#include "Application.h"
#include "Assets/AssetStreamer.h"
#include "Renderer/Common/Constants.h"
#include "Renderer/Renderer.h"
#include "Services/Filesystem.h"

using namespace Silent::Renderer;
using namespace Silent::Services;

namespace Silent::Assets
{
    std::shared_ptr<void> PngParse(const std::filesystem::path& filename)
    {
        const auto& fs = g_App.GetFilesystem();

        // Create asset.
        auto asset = PngAsset{};

        // Read pixels.
        int    channels = 0;
        uchar* pixels   = stbi_load(filename.string().c_str(), &asset.Resolution.x, &asset.Resolution.y, &channels, RGBA_COMP_COUNT);
        if (pixels == nullptr) 
        {
            throw std::runtime_error(Fmt("Failed to open PNG `{}`.",
                                         std::filesystem::relative(filename, fs.GetAssetsDirectory()).string()));
        }

        // Copy pixels.
        asset.Pixels      = std::vector<byte>(pixels, pixels + ((asset.Resolution.x * asset.Resolution.y) * RGBA_COMP_COUNT));
        asset.AspectRatio = (float)asset.Resolution.x / (float)asset.Resolution.y;

        // Free resources.
        stbi_image_free(pixels);

        return std::make_shared<PngAsset>(std::move(asset));
    }

    void PngQueueGpuUpload(const Asset& asset)
    {
        auto& renderer = g_App.GetRenderer();

        renderer.QueueTextureUpload(asset.Name);
    }

    void PngQueueGpuRelease(const Asset& asset)
    {
        auto& renderer = g_App.GetRenderer();

        renderer.QueueTextureRelease(asset.Name);
    }
}
