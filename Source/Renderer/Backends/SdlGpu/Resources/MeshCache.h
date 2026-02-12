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

        /** @brief Creates an instance from a reverence 3D GPU vertex buffer.
         *
         * @param vertBuffer 3D GPU vertex buffer.
         */
        MeshCache(VertexBuffer<BufferVertex3d>& vertBuffer);

        /** @brief Loads a mesh from collections of vertices and indices.
         *
         * @param copyPass Copy pass.
         * @param verts 3D vertices.
         * @param idxs 3D vertex indices.
         * @param name Mesh name.
         */
        void Load(SDL_GPUCopyPass& copyPass,
                  const std::vector<BufferVertex3d>& verts, const std::vector<uint16>& idxs,
                  const std::string& name);

        /** @brief Loads a mesh from a streamable asset.
         *
         * @param copyPass Copy pass.
         * @param assetName Streamable asset name.
         */
        void Load(SDL_GPUCopyPass& copyPass, const std::string& assetName);

    private:
        // ========
        // Helpers
        // ========

        /** @brief Loads a mesh from an ILM asset.
         *
         * @param copyPass Copy pass.
         * @param asset ILM asset.
         */
        void LoadIlm(SDL_GPUCopyPass& copyPass, const Asset& asset);

        /** @brief Loads a mesh from a TMD asset.
         *
         * @param copyPass Copy pass.
         * @param asset TMD asset.
         */
        void LoadTmd(SDL_GPUCopyPass& copyPass, const Asset& asset);
    };
}
