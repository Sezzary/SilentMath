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
    bool Mesh::IsValid() const
    {
        return VertexOffset != NO_VALUE && IdxOffset != NO_VALUE;
    }

    void MeshCacheBase::Unload(const std::string& name)
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

        // Remove mesh.
        _meshes.erase(name);
    }

    void MeshCacheBase::UnloadModel(const std::string& assetName)
    {
        auto& assets = g_App.GetAssets();

        // Get asset.
        const auto* asset = assets.GetAsset(assetName);
        if (asset == nullptr)
        {
            Debug::Log(Fmt("Attempted to unload GPU meshes from invalid asset `{}`.", asset->Name), Debug::LogLevel::Error);
        }

        // Unload meshes from model asset.
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
                Debug::Log(Fmt("Attempted to unload GPU meshes from non-mesh asset `{}`.", asset->Name),
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
        // Check if mesh exists.
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

        for (int i = 0; i < data->Meshes.size(); i++)
        {
            const auto& mesh = data->Meshes[i];
            Unload(asset.Name + "_" + mesh.BoneName);
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

        for (int i = 0; i < data->Meshes.size(); i++)
        {
            const auto& mesh = data->Meshes[i];
            Unload(asset.Name + "_" + std::to_string(i));
        }
    }
}
