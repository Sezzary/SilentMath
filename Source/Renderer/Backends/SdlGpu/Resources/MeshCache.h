#pragma once

#include "Assets/AssetStreamer.h"
#include "Renderer/Common/Resources/MeshCache.h"

using namespace Silent::Assets;

namespace Silent::Renderer{ struct BufferVertex3d; }

namespace Silent::Renderer::SdlGpu
{
    template <typename T> class VertexBuffer;

    /** @brief GPU mesh cache. */
    class MeshCache : public MeshCacheBase
    {
    private:
        // =======
        // Fields
        // =======

        VertexBuffer<BufferVertex3d>* _vertexBuffer = nullptr;

    public:
        // =============
        // Constructors
        // =============

        /** @brief Creates an instance from a reference GPU 3D vertex buffer.
         *
         * @param vertBuffer GPU 3D vertex buffer.
         */
        MeshCache(VertexBuffer<BufferVertex3d>& vertBuffer);

        /** @brief Loads a mesh from vertices and indices.
         *
         * @param copyPass Copy pass.
         * @param verts 3D vertices.
         * @param idxs 3D vertex indices.
         * @param name Mesh name.
         */
        void Load(SDL_GPUCopyPass& copyPass,
                  const std::vector<BufferVertex3d>& verts, const std::vector<uint16>& idxs,
                  const std::string& name);

        /** @brief Loads model meshes from a streamable model asset.
         *
         * @param copyPass Copy pass.
         * @param assetName Streamable model asset name.
         */
        void Load(SDL_GPUCopyPass& copyPass, const std::string& assetName);

    private:
        // ========
        // Helpers
        // ========

        /** @brief Loads GPU meshes from an ILM model asset.
         *
         * @note By convention, each GPUmesh is named as follows:
         * `[model name]_[bone name]_[bone mesh variant index]`.
         * @todo Also append bone mesh variant index to name.
         *
         * @param copyPass Copy pass.
         * @param asset ILM asset.
         */
        void LoadIlm(SDL_GPUCopyPass& copyPass, const Asset& asset);

        /** @brief Loads GPU meshes from an IPD model asset.
         *
         * @param copyPass Copy pass.
         * @param asset IPD asset.
         */
        void LoadIpd(SDL_GPUCopyPass& copyPass, const Asset& asset);

        /** @brief Loads GPU meshes from a PLM model asset.
         *
         * @param copyPass Copy pass.
         * @param asset PLM asset.
         */
        void LoadPlm(SDL_GPUCopyPass&, const Asset& asset);

        /** @brief Loads GPU meshes from a TMD model asset.
         *
         * @note By convention, each GPU mesh is named as follows:
         * `[model name]_[mesh index]`.
         *
         * @param copyPass Copy pass.
         * @param asset TMD asset.
         */
        void LoadTmd(SDL_GPUCopyPass& copyPass, const Asset& asset);
    };
}
