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
    std::vector<std::string> MeshCacheBase::GetNames() const
    {
        // Collect sorted mesh names.
        auto names = std::vector<std::string>{};
        names.reserve(_meshes.size());
        for (const auto& [keyName, mesh] : _meshes)
        {
            names.push_back(keyName);
        }
        Sort(names);

        return names;
    }

    void MeshCacheBase::Release(const std::string& name)
    {
        // Check if mesh name exists.
        const auto* mesh = Find(_meshes, name);
        if (mesh == nullptr)
        {
            return;
        }

        // Dellocate memory blocks for vertices and indices.
        _vertexAllocator.Deallocate(mesh->VertexOffset);
        _idxAllocator.Deallocate(mesh->IdxOffset);

        // Remove mesh allocation.
        _meshes.erase(name);
    }

    void MeshCacheBase::ReleaseModel(const std::string& assetName)
    {
        auto& assets = g_App.GetAssets();

        // Get asset.
        const auto* asset = assets[assetName];
        if (asset == nullptr)
        {
            Debug::Log(Fmt("Attempted to unload GPU meshes from invalid asset `{}`.", asset->Name),
                       Debug::LogLevel::Error);
        }

        // Unload meshes from model asset.
        switch (asset->Type)
        {
            case AssetType::Ilm:
            case AssetType::Plm:
            {
                ReleaseLm(*asset);
                break;
            }
            case AssetType::Ipd:
            {
                ReleaseIpd(*asset);
                break;
            }
            case AssetType::Tmd:
            {
                ReleaseTmd(*asset);
                break;
            }
            default:
            {
                Debug::Log(Fmt("Attempted to unload GPU meshes from non-mesh asset `{}`.", asset->Name),
                           Debug::LogLevel::Error);
                break;
            }
        }
    }

    const Mesh* MeshCacheBase::operator[](const std::string& name) const
    {
        // Check if mesh exists.
        const auto* mesh = Find(_meshes, name);
        if (mesh == nullptr)
        {
            Debug::Log(Fmt("Attempted to get missing GPU mesh `{}`.", name), Debug::LogLevel::Warning);
            return nullptr;
        }

        return &*mesh;
    }

    void MeshCacheBase::ReleaseLm(const Asset& asset)
    {
        const auto data = asset.GetData<LmAsset>();

        for (int i = 0; i < data->Meshes.size(); i++)
        {
            const auto& mesh = data->Meshes[i];
            Release(asset.Name + "_" + mesh.BoneName);
        }
    }

    void MeshCacheBase::ReleaseIpd(const Asset& asset)
    {
        const auto data = asset.GetData<IpdAsset>();

        // @todo
    }

    void MeshCacheBase::ReleaseTmd(const Asset& asset)
    {
        const auto data = asset.GetData<TmdAsset>();

        for (int i = 0; i < data->Meshes.size(); i++)
        {
            const auto& mesh = data->Meshes[i];
            Release(asset.Name + "_" + std::to_string(i));
        }
    }
}
