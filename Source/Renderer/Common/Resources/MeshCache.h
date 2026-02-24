#pragma once

#include "Assets/AssetStreamer.h"
#include "Utils/BlockAllocator.h"

using namespace Silent::Assets;
using namespace Silent::Utils;

namespace Silent::Renderer
{
    /** @brief GPU mesh allocation. */
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

        /** @brief Releases a cached mesh from the GPU.
         *
         * @param name Name of the GPU mesh to unload.
         */
        void Release(const std::string& name);

        /** @brief Releases meshes of a streamable model asset from the GPU.
         *
         * @param assetName Streamable model asset name.
         */
        void ReleaseModel(const std::string& assetName);

        /** @brief Releases all cached meshes from the GPU. */
        void Clear();

        // ==========
        // Operators
        // ==========

        const Mesh* operator[](const std::string& name) const;

    private:
        /** @brief Releases meshes of an ILM model asset from the GPU.
         *
         * @note By convention, each GPUmesh is named as follows:
         * `[ILM asset name]_[bone name]_[bone mesh variant index]`.
         *
         * @param asset ILM asset.
         */
        void ReleaseIlm(const Asset& asset);

        /** @brief Releases meshes of an IPD model asset from the GPU.
         *
         * @todo Mesh naming convention note.
         *
         * @param asset IPD asset.
         */
        void ReleaseIpd(const Asset& asset);

        /** @brief Releases meshes of a TMD model asset from the GPU.
         *
         * @note Each GPU mesh uses the following naming convention:
         * `[TMD asset name]_[mesh index]`.
         *
         * @param asset TMD asset.
         */
        void ReleaseTmd(const Asset& asset);
    };
}
