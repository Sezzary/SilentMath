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

    void MeshCache::Load(SDL_GPUCopyPass& copyPass, const std::string& assetName)
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
                LoadIlm(copyPass, asset);
                break;
            }
            case AssetType::Tmd:
            {
                LoadTmd(copyPass, asset);
                break;
            }
            default:
            {
                Debug::Log(Fmt("Attempted to load non-image asset `{}` as GPU mesh.", asset->Name), Debug::LogLevel::Error);
                break;
            }
        }
    }

    void MeshCache::LoadIlm(SDL_GPUCopyPass& copyPass, std::shared_ptr<Asset> asset)
    {
        // @todo
    }

    void MeshCache::LoadTmd(SDL_GPUCopyPass& copyPass, std::shared_ptr<Asset> asset)
    {
        const auto data = asset->GetData<TmdAsset>();

        // Run through meshes.
        for (int i = 0; i < data->Meshes.size(); i++)
        {
            const auto& mesh = data->Meshes[i];

            // Build vertices and indices.
            auto verts = std::vector<BufferVertex3d>{};
            auto idxs  = std::vector<uint16>{};
            for (const auto& prim : mesh.Primitives)
            {
                auto primIdxs = std::vector<uint16>{};

                // @todo Deduplicate. Later, TMDs and other model formats should be parsed to GPU-ready data.
                // Collect vertices.
                for (const auto& tmdVert : prim.Vertices)
                {
                    uint16 newIdx = (uint16)verts.size();
                    verts.push_back(BufferVertex3d
                    {
                        .Position = mesh.Positions[tmdVert.PositionIdx],
                        .Normal   = mesh.Normals[tmdVert.NormalIdx],
                        .Uv       = mesh.Uvs[tmdVert.UvIdx],
                        .Col      = mesh.Colors[tmdVert.ColorIdx]
                    });
                    primIdxs.push_back(newIdx);
                }

                // Collect indices.
                if (primIdxs.size() == TRI_IDX_COUNT)
                {
                    idxs.insert(idxs.end(),
                    {
                        primIdxs[0], primIdxs[1], primIdxs[2]
                    });
                }
                else if (primIdxs.size() == QUAD_IDX_COUNT)
                {
                    idxs.insert(idxs.end(),
                    {
                        primIdxs[0], primIdxs[1], primIdxs[2], 
                        primIdxs[0], primIdxs[2], primIdxs[3]
                    });
                }
            }

            int vertOffset = _vertexAllocator.Allocate(verts.size());
            int idxOffset  = _idxAllocator.Allocate(idxs.size());

            _meshes.try_emplace(asset->Name/* + std::to_string(i)*/, Mesh
            {
                .VertexOffset = (uint32)vertOffset,
                .IdxOffset    = (uint32)idxOffset,
                .IdxCount     = (uint32)idxs.size()
            });

            _vertexBuffer->UpdateVertices(copyPass, ToSpan(verts), vertOffset);
            _vertexBuffer->UpdateIdxs(copyPass, ToSpan(idxs), idxOffset);

            // @todo Just the first mesh for now.
            break;
        }
    }
}
