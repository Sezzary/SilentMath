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
        int vertOffset = _vertexAllocator.Allocate(verts.size());
        int idxOffset  = _idxAllocator.Allocate(idxs.size());

        _meshes.try_emplace(name, Mesh
        {
            .VertexOffset = (uint32)vertOffset,
            .IdxOffset    = (uint32)idxOffset,
            .IdxCount     = (uint32)idxs.size()
        });

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
            Debug::Log(Fmt("Attempted to load invalid asset `{}` as GPU mesh.", asset->Name), Debug::LogLevel::Error);
        }

        // Load mesh asset.
        switch (asset->Type)
        {
            case AssetType::Ilm:
            {
                LoadIlm(copyPass, *asset);
                break;
            }
            case AssetType::Tmd:
            {
                LoadTmd(copyPass, *asset);
                break;
            }
            default:
            {
                Debug::Log(Fmt("Attempted to load non-image asset `{}` as GPU mesh.", asset->Name),
                           Debug::LogLevel::Error);
                break;
            }
        }
    }

    void MeshCache::LoadIlm(SDL_GPUCopyPass& copyPass, const Asset& asset)
    {
        const auto data = asset.GetData<IlmAsset>();

        for (int i = 0; i < data->LinearMeshes.size(); i++)
        {
            const auto& mesh = data->LinearMeshes[i];
            Load(copyPass, mesh.Vertices, mesh.Idxs, asset.Name + std::to_string(i));
        }
    }

    void MeshCache::LoadTmd(SDL_GPUCopyPass& copyPass, const Asset& asset)
    {
        const auto data = asset.GetData<TmdAsset>();

        for (int i = 0; i < data->LinearMeshes.size(); i++)
        {
            const auto& mesh = data->LinearMeshes[i];
            Load(copyPass, mesh.Vertices, mesh.Idxs, asset.Name + std::to_string(i));
        }
    }
}
