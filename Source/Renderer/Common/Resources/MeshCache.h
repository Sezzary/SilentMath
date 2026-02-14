#pragma once

#include "Assets/AssetStreamer.h"
#include "Utils/BlockAllocator.h"

using namespace Silent::Assets;
using namespace Silent::Utils;

namespace Silent::Renderer
{
    /** @brief GPU mesh. */
    struct Mesh
    {
        int VertexOffset = 0;
        int IdxOffset    = 0;
        int IdxCount     = 0;

        bool IsValid() const;
    };

    /** @brief GPU mesh cache base. */
    class MeshCacheBase
    {
    protected:
        // =======
        // Fields
        // =======

        std::unordered_map<std::string, Mesh> _meshes          = {}; /** Key = mesh name, value = GPU mesh. */
        BlockAllocator                        _vertexAllocator = BlockAllocator();
        BlockAllocator                        _idxAllocator    = BlockAllocator();

    public:
        // ==========
        // Utilities
        // ==========

        /** @brief Unloads a cached mesh.
         *
         * @param name Name of the mesh to unload.
         */
        void Unload(const std::string& name);

        /** @brief Unloads model meshes of a streamable model asset.
         *
         * @param assetName Streamable model asset.
         */
        void UnloadAssetModel(const std::string& assetName);

        /** @brief Clears all cached meshes. */
        void Clear();

        // ==========
        // Operators
        // ==========

        const Mesh* operator[](const std::string& name) const;

    private:
        void UnloadIlm(const Asset& asset);

        void UnloadIpd(const Asset& asset);

        void UnloadTmd(const Asset& asset);
    };
}
