#include "Framework.h"
#include "Assets/Loaders/Plm.h"

#include "Application.h"
#include "Assets/AssetStreamer.h"
#include "Assets/Loaders/Utils/Lm.h"
#include "Utils/Stream.h"
#include "Utils/Utils.h"

using namespace Silent::Utils;

namespace Silent::Assets
{
    std::shared_ptr<void> ParsePlm(const stdfs::path& filename)
    {
        // @todo
        return nullptr;
    }

    void QueuePlmGpuUpload(const Asset& asset)
    {
        auto& renderer = g_App.GetRenderer();

        renderer.QueueMeshUpload(asset.Name);
    }

    void QueuePlmGpuRelease(const Asset& asset)
    {

        auto& renderer = g_App.GetRenderer();

        renderer.QueueMeshRelease(asset.Name);
    }
}
