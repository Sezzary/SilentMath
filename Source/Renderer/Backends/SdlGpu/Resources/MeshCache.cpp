#include "Framework.h"
#include "Renderer/Backends/SdlGpu/Resources/MeshCache.h"

#include "Application.h"
#include "Assets/AssetStreamer.h"
#include "Renderer/Backends/SdlGpu/Resources/VertexBuffer.h"
#include "Renderer/Common/Constants.h"
#include "Renderer/Common/Resources/Layouts/Buffers.h"
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
                           std::span<const BufferVertex3d> verts, std::span<const uint16> idxs,
                           const std::string& name)
    {
        // Check if mesh with same name already exists.
        if (Find(_meshes, name) != nullptr)
        {
            Debug::Log(Fmt("Attempted to upload existing GPU mesh `{}`.", name), Debug::LogLevel::Warning);
            return;
        }

        // Allocate memory blocks for vertices and indices.
        int vertOffset = _vertexAllocator.Allocate(verts.size());
        int idxOffset  = _idxAllocator.Allocate(idxs.size());

        // Check for invalid block allocation.
        if (vertOffset == NO_VALUE || idxOffset == NO_VALUE)
        {
            // Cleanup.
            if (vertOffset != NO_VALUE)
            {
                _vertexAllocator.Deallocate(verts.size());
            }
            if (idxOffset != NO_VALUE)
            {
                _idxAllocator.Deallocate(idxs.size());
            }

            Debug::Log(Fmt("Attempted to upload GPU mesh `{}` with not enough memory.", name), Debug::LogLevel::Error);
            return;
        }

        // Insert mesh.
        _meshes.try_emplace(name, Mesh
        {
            .VertexOffset = vertOffset,
            .IdxOffset    = idxOffset,
            .IdxCount     = (int)idxs.size()
        });

        // Update GPU vertex buffer.
        _vertexBuffer.UpdateVertices(copyPass, verts, vertOffset);
        _vertexBuffer.UpdateIdxs(copyPass, idxs, idxOffset);
    }

    void MeshCache::Upload(SDL_GPUCopyPass& copyPass, const std::string& assetName)
    {
        auto& assets = g_App.GetAssets();

        // Get asset.
        const auto* asset = assets[assetName];
        if (asset == nullptr)
        {
            Debug::Log(Fmt("Attempted to upload GPU meshes from invalid asset `{}`.", asset->Name),
                       Debug::LogLevel::Error);
            return;
        }

        // Upload model meshes from asset.
        switch (asset->Type)
        {
            case AssetType::Ilm:
            case AssetType::Ipd:
            case AssetType::Plm:
            {
                UploadLm(copyPass, *asset);
                break;
            }
            case AssetType::Tmd:
            {
                UploadTmd(copyPass, *asset);
                break;
            }
            default:
            {
                Debug::Log(Fmt("Attempted to upload GPU meshes from non-model asset `{}`.", asset->Name),
                           Debug::LogLevel::Error);
                break;
            }
        }
    }

    void MeshCache::Bind(SDL_GPURenderPass& renderPass)
    {
        _vertexBuffer.Bind(renderPass, 0, 0);
    }

    void MeshCache::ReleaseAll()
    {
        _meshes.clear();
        _vertexAllocator.Clear();
        _idxAllocator.Clear();
        _vertexBuffer.Release();
    }

    void MeshCache::UploadLm(SDL_GPUCopyPass& copyPass, const Asset& asset)
    {
        const std::vector<LmMesh>* meshes = nullptr;
        switch (asset.Type)
        {
            case AssetType::Ilm:
            {
                const auto data = asset.GetData<IlmAsset>();
                meshes          = &data->Lm.Meshes;
                break;
            }
            case AssetType::Ipd:
            {
                const auto data = asset.GetData<IpdAsset>();
                meshes          = &data->Lm.Meshes;
                break;
            }
            case AssetType::Plm:
            {
                const auto data = asset.GetData<PlmAsset>();
                meshes          = &data->Lm.Meshes;
                break;
            }
            default:
            {
                return;
            }
        }

        for (const auto& mesh : *meshes)
        {
            Upload(copyPass, ToSpan(mesh.Linear.Vertices), ToSpan(mesh.Linear.Idxs), asset.Name + "_" + mesh.Name);
        }
    }

    void MeshCache::UploadTmd(SDL_GPUCopyPass& copyPass, const Asset& asset)
    {
        const auto data = asset.GetData<TmdAsset>();

        for (int i = 0; i < data->Meshes.size(); i++)
        {
            const auto& mesh = data->Meshes[i];
            Upload(copyPass, ToSpan(mesh.Linear.Vertices), ToSpan(mesh.Linear.Idxs), asset.Name + "_" + std::to_string(i));
        }
    }
}
