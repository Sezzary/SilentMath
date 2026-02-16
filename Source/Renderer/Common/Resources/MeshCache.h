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

        /** @brief Unloads a cached GPU mesh.
         *
         * @param name Name of the GPU mesh to unload.
         */
        void Unload(const std::string& name);

        /** @brief Unloads GPU meshes from a streamable model asset.
         *
         * @param assetName Streamable model asset name.
         */
        void UnloadModel(const std::string& assetName);

        /** @brief Clears all cached GPU meshes. */
        void Clear();

        // ==========
        // Operators
        // ==========

        const Mesh* operator[](const std::string& name) const;

    private:
        /** @brief Unloads GPU meshes from an ILM model asset.
         *
         * @note By convention, each GPUmesh is named as follows:
         * `[model name]_[bone name]_[bone mesh variant index]`.
         *
         * @param asset ILM asset.
         */
        void UnloadIlm(const Asset& asset);

        /** @brief Unloads GPU meshes from an IPD model asset.
         *
         * @todo Note.
         *
         * @param asset IPD asset.
         */
        void UnloadIpd(const Asset& asset);

        /** @brief Unloads GPU meshes from a TMD model asset.
         *
         * @note By convention, each GPU mesh is named as follows:
         * `[model name]_[mesh index]`.
         *
         * @param asset TMD asset.
         */
        void UnloadTmd(const Asset& asset);
    };
}
