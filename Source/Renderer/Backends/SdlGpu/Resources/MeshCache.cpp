#include "Framework.h"
#include "Renderer/Backends/SdlGpu/Resources/MeshCache.h"

#include "Application.h"
#include "Assets/AssetStreamer.h"
#include "Renderer/Backends/SdlGpu/Resources/VertexBuffer.h"
#include "Renderer/Common/Constants.h"
#include "Renderer/Common/Resources/Buffers.h"
#include "Renderer/Common/Resources/MeshCache.h"
#include "Utils/Utils.h"

using namespace Silent::Assets;
using namespace Silent::Utils;

namespace Silent::Renderer::SdlGpu
{
    MeshCache::MeshCache(VertexBuffer<BufferVertex3d>& vertBuffer)
    {
        _vertexBuffer    = &vertBuffer;
        _vertexAllocator = BlockAllocator(vertBuffer.GetVertexCapacity());
        _idxAllocator    = BlockAllocator(vertBuffer.GetIdxCapacity());
    }

    void MeshCache::Load(SDL_GPUCopyPass& copyPass,
                         const std::vector<BufferVertex3d>& verts, const std::vector<uint16>& idxs,
                         const std::string& name)
    {
        // Check if mesh with same name already exists.
        if (Find(_meshes, name) != nullptr)
        {
            Debug::Log(Fmt("Attempted to overwrite existing GPU mesh `{}`.", name), Debug::LogLevel::Warning);
            return;
        }

        // Allocate memory blocks for vertices and indices.
        int vertOffset = _vertexAllocator.Allocate(verts.size());
        int idxOffset  = _idxAllocator.Allocate(idxs.size());

        // Insert mesh
        _meshes.emplace(name, Mesh
        {
            .VertexOffset = vertOffset,
            .IdxOffset    = idxOffset,
            .IdxCount     = (int)idxs.size()
        });

        // Update GPU vertex buffer.
        _vertexBuffer->UpdateVertices(copyPass, ToSpan(verts), vertOffset);
        _vertexBuffer->UpdateIdxs(copyPass, ToSpan(idxs), idxOffset);
    }

    void MeshCache::Load(SDL_GPUCopyPass& copyPass, const std::string& assetName)
    {
        auto& assets = g_App.GetAssets();

        // Get asset.
        const auto* asset = assets.GetAsset(assetName);
        if (asset == nullptr)
        {
            Debug::Log(Fmt("Attempted to load GPU meshes from invalid asset `{}`.", asset->Name),
                       Debug::LogLevel::Error);
            return;
        }

        // Load model meshes from asset.
        switch (asset->Type)
        {
            case AssetType::Ilm:
            {
                LoadIlm(copyPass, *asset);
                break;
            }
            case AssetType::Ipd:
            {
                LoadIpd(copyPass, *asset);
                break;
            }
            case AssetType::Plm:
            {
                LoadPlm(copyPass, *asset);
                break;
            }
            case AssetType::Tmd:
            {
                LoadTmd(copyPass, *asset);
                break;
            }
            default:
            {
                Debug::Log(Fmt("Attempted to load GPU meshes from non-model asset `{}`.", asset->Name),
                           Debug::LogLevel::Error);
                break;
            }
        }
    }

    void MeshCache::LoadIlm(SDL_GPUCopyPass& copyPass, const Asset& asset)
    {
        const auto data = asset.GetData<IlmAsset>();

        for (int i = 0; i < data->Meshes.size(); i++)
        {
            const auto& mesh = data->Meshes[i];
            Load(copyPass, mesh.Linear.Vertices, mesh.Linear.Idxs, asset.Name + "_" + mesh.BoneName);
        }
    }

    void MeshCache::LoadPlm(SDL_GPUCopyPass& copyPass, const Asset& asset)
    {
        const auto data = asset.GetData<PlmAsset>();

        // @todo
    }

    void MeshCache::LoadIpd(SDL_GPUCopyPass& copyPass, const Asset& asset)
    {
        const auto data = asset.GetData<IpdAsset>();

        // @todo
    }

    void MeshCache::LoadTmd(SDL_GPUCopyPass& copyPass, const Asset& asset)
    {
        const auto data = asset.GetData<TmdAsset>();

        for (int i = 0; i < data->Meshes.size(); i++)
        {
            const auto& mesh = data->Meshes[i];
            Load(copyPass, mesh.Linear.Vertices, mesh.Linear.Idxs, asset.Name + "_" + std::to_string(i));
        }
    }
}
