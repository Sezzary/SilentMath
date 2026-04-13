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
    MeshCache::MeshCache(SDL_GPUDevice& device, int vertCount, int idxCount, const std::string& name)
    {
        _vertexBuffer.Initialize(device, vertCount, idxCount, name);
        _vertexAllocator = BlockAllocator(_vertexBuffer.GetVertexCapacity());
        _idxAllocator    = BlockAllocator(_vertexBuffer.GetIdxCapacity());
    }

    void MeshCache::Upload(SDL_GPUCopyPass& copyPass,
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

        // Insert mesh.
        _meshes.try_emplace(name, Mesh
        {
            .VertexOffset = vertOffset,
            .IdxOffset    = idxOffset,
            .IdxCount     = (int)idxs.size()
        });

        // Update GPU vertex buffer.
        _vertexBuffer.UpdateVertices(copyPass, ToSpan(verts), vertOffset);
        _vertexBuffer.UpdateIdxs(copyPass, ToSpan(idxs), idxOffset);
    }

    void MeshCache::Upload(SDL_GPUCopyPass& copyPass, const std::string& assetName)
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

        // Upload model meshes from asset.
        switch (asset->Type)
        {
            case AssetType::Ilm:
            {
                UploadIlm(copyPass, *asset);
                break;
            }
            case AssetType::Ipd:
            {
                UploadIpd(copyPass, *asset);
                break;
            }
            case AssetType::Plm:
            {
                UploadPlm(copyPass, *asset);
                break;
            }
            case AssetType::Tmd:
            {
                UploadTmd(copyPass, *asset);
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

    void MeshCache::Bind(SDL_GPURenderPass& renderPass)
    {
        _vertexBuffer.Bind(renderPass, 0, 0);
    }

    void MeshCache::UploadIlm(SDL_GPUCopyPass& copyPass, const Asset& asset)
    {
        const auto data = asset.GetData<IlmAsset>();

        for (int i = 0; i < data->Meshes.size(); i++)
        {
            const auto& mesh = data->Meshes[i];
            Upload(copyPass, mesh.Linear.Vertices, mesh.Linear.Idxs, asset.Name + "_" + mesh.BoneName);
        }
    }

    void MeshCache::UploadPlm(SDL_GPUCopyPass& copyPass, const Asset& asset)
    {
        const auto data = asset.GetData<PlmAsset>();

        // @todo
        Debug::Log("Attempted to load PLM GPU meshes. Unimplemented.", Debug::LogLevel::Warning);
    }

    void MeshCache::UploadIpd(SDL_GPUCopyPass& copyPass, const Asset& asset)
    {
        const auto data = asset.GetData<IpdAsset>();

        // @todo
        Debug::Log("Attempted to load IPD GPU meshes. Unimplemented.", Debug::LogLevel::Warning);
    }

    void MeshCache::UploadTmd(SDL_GPUCopyPass& copyPass, const Asset& asset)
    {
        const auto data = asset.GetData<TmdAsset>();

        for (int i = 0; i < data->Meshes.size(); i++)
        {
            const auto& mesh = data->Meshes[i];
            Upload(copyPass, mesh.Linear.Vertices, mesh.Linear.Idxs, asset.Name + "_" + std::to_string(i));
        }
    }
}
