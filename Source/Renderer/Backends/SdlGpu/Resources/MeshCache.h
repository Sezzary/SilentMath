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

        /** @brief Constructs a `MeshCache` for a reverence GPU 3D vertex buffer.
         *
         * @param vertBuffer GPU vertex buffer.
         */
        MeshCache(VertexBuffer<BufferVertex3d>& vertBuffer);

        /** @brief Loads a mesh from a streamable asset.
         *
         * @todo Find best way handle loading multiple meshes from a model.
         *
         * @param copyPass Copy pass.
         * @param assetName Streamable asset name.
         */
        void Load(SDL_GPUCopyPass& copyPass, const std::string& assetName);

    private:
        // ========
        // Helpers
        // ========

        void LoadIlm(SDL_GPUCopyPass& copyPass, std::shared_ptr<Asset> asset);

        void LoadTmd(SDL_GPUCopyPass& copyPass, std::shared_ptr<Asset> asset);
    };
}
