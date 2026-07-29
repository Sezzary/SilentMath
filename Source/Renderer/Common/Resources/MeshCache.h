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
        // ========
        // Getters
        // ========

        /** @brief Gets the names of meshes cached on the GPU, sorted in alphanumerically.
         *
         * @return Mesh names.
         */
        std::vector<std::string> GetNames() const;

        // ==========
        // Utilities
        // ==========

        /** @brief Releases a cached mesh from the GPU.
         *
         * @param name Mesh name.
         */
        void Release(const std::string& name);

        /** @brief Releases cached meshes of a streamable model asset from the GPU.
         *
         * @param assetName Streamable model asset name.
         */
        void ReleaseModel(const std::string& assetName);

        // ==================
        // Virtual Utilities
        // ==================

        /** @brief Releases all cached meshes from the GPU.
         *
         * @note Due to C++ inheritance quirks, this cannot simply be named `Release`.
         */
        virtual void ReleaseAll() = 0;

        // ==========
        // Operators
        // ==========

        const Mesh* operator[](const std::string& name) const;

    private:
        /** @brief Releases meshes of an LM chunk from the GPU.
         *
         * @note By convention, GPU meshes use the following naming pattern:
         * `[asset name]_[bone name]`.
         *
         * @param asset ILM, PLM, or IPD asset.
         */
        void ReleaseLm(const Asset& asset);

        /** @brief Releases meshes of a TMD model asset from the GPU.
         *
         * @note By convention, GPU meshes use the following naming pattern:
         * `[asset name]_[mesh index]`.
         *
         * @param asset TMD asset.
         */
        void ReleaseTmd(const Asset& asset);
    };
}
