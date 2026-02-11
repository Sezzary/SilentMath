#include "Framework.h"
#include "Renderer/Backends/SdlGpu/Resources/MeshCache.h"

#include "Application.h"
#include "Assets/AssetStreamer.h"
#include "Renderer/Common/Resources/MeshCache.h"

using namespace Silent::Assets;

namespace Silent::Renderer
{
    MeshCache::MeshCache(int vertCount, int idxCount)
    {
        _vertexAllocator = BlockAllocator(vertCount);
        _idxAllocator    = BlockAllocator(idxCount);
    }

    void MeshCache::Load(const std::string& assetName)
    {
        auto& assets = g_App.GetAssets();

        // Get asset.
        const auto asset = assets.GetAsset(assetName);
        if (asset == nullptr)
        {
            Debug::Log(Fmt("Attempted to load invalid asset `{}` as GPU mesh.", asset->Name), Debug::LogLevel::Error);
        }

        // Load mesh asset.
        switch (asset->Type)
        {
            case AssetType::Ilm:
            {
                //LoadIlm(copyPass, asset);
                break;
            }
            case AssetType::Tmd:
            {
                //LoadTmd(copyPass, asset);
                break;
            }
            default:
            {
                Debug::Log(Fmt("Attempted to load non-image asset `{}` as GPU texture.", asset->Name), Debug::LogLevel::Error);
                break;
            }
        }
    }
}
