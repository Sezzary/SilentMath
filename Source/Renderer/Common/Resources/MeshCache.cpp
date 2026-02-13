#include "Framework.h"
#include "Renderer/Common/Resources/MeshCache.h"

#include "Application.h"
#include "Assets/AssetStreamer.h"
#include "Utils/BlockAllocator.h"
#include "Utils/Utils.h"

using namespace Silent::Assets;
using namespace Silent::Utils;

namespace Silent::Renderer
{
    void MeshCacheBase::Unload(const std::string& name)
    {
        const auto* mesh = Find(_meshes, name);
        if (mesh == nullptr)
        {
            return;
        }

        _vertexAllocator.Deallocate(mesh->VertexOffset);
        _idxAllocator.Deallocate(mesh->IdxOffset);
        _meshes.erase(name);
    }

    void MeshCacheBase::UnloadAssetModel(const std::string& assetName)
    {
        auto& assets = g_App.GetAssets();

        // Get asset.
        const auto* asset = assets.GetAsset(assetName);
        if (asset == nullptr)
        {
            Debug::Log(Fmt("Attempted to unload invalid asset `{}` as GPU mesh.", asset->Name), Debug::LogLevel::Error);
        }

        // Unload model asset meshes.
        switch (asset->Type)
        {
            case AssetType::Ilm:
            {
                UnloadIlm(*asset);
                break;
            }
            case AssetType::Ipd:
            {
                UnloadIpd(*asset);
                break;
            }
            case AssetType::Tmd:
            {
                UnloadTmd(*asset);
                break;
            }
            default:
            {
                Debug::Log(Fmt("Attempted to unload non-mesh asset `{}` as GPU mesh.", asset->Name),
                           Debug::LogLevel::Error);
                break;
            }
        }
    }

    void MeshCacheBase::Clear()
    {
        _meshes.clear();
        _vertexAllocator.Clear();
        _idxAllocator.Clear();
    }

    const Mesh* MeshCacheBase::operator[](const std::string& name) const
    {
        const auto* mesh = Find(_meshes, name);
        if (mesh == nullptr)
        {
            Debug::Log(Fmt("Attempted to get missing GPU mesh `{}`.", name), Debug::LogLevel::Warning);
            return nullptr;
        }

        return &*mesh;
    }

    void MeshCacheBase::UnloadIlm(const Asset& asset)
    {
        const auto data = asset.GetData<IlmAsset>();

        for (int i = 0; i < data->LinearMeshes.size(); i++)
        {
            const auto& mesh = data->LinearMeshes[i];
            Unload(mesh.BoneName);
        }
    }

    void MeshCacheBase::UnloadIpd(const Asset& asset)
    {
        const auto data = asset.GetData<IpdAsset>();

        // @todo
    }

    void MeshCacheBase::UnloadTmd(const Asset& asset)
    {
        const auto data = asset.GetData<TmdAsset>();

        for (int i = 0; i < data->LinearMeshes.size(); i++)
        {
            const auto& mesh = data->LinearMeshes[i];
            Unload(asset.Name + std::to_string(i));
        }
    }
}
