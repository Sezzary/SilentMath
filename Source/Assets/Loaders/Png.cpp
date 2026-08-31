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
    std::shared_ptr<void> ParsePng(const stdfs::path& filename)
    {
        const auto& fs = g_App.GetFilesystem();

        // Read image.
        auto   res      = Vector2i::Zero;
        int    channels = 0;
        uchar* pixels   = stbi_load(filename.string().c_str(), &res.x, &res.y, &channels, RGBA_COMP_COUNT);
        if (pixels == nullptr) 
        {
            throw std::runtime_error(Fmt("Failed to open PNG `{}`.",
                                         stdfs::relative(filename, fs.GetAssetsDirectory()).string()));
        }

        // Copy pixels.
        auto pixelsCpy = std::vector<byte>(pixels, pixels + ((res.x * res.y) * RGBA_COMP_COUNT));

        // Free resources.
        stbi_image_free(pixels);

        return std::make_shared<PngAsset>(PngAsset
        {
            .Resolution  = res,
            .Pixels      = std::move(pixelsCpy),
            .AspectRatio = (float)res.x / (float)res.y
        });
    }

    void QueuePngGpuUpload(const Asset& asset)
    {
        auto& renderer = g_App.GetRenderer();

        renderer.QueueTextureUpload(asset.Name);
    }

    void QueuePngGpuRelease(const Asset& asset)
    {
        auto& renderer = g_App.GetRenderer();

        renderer.QueueTextureRelease(asset.Name);
    }
}
