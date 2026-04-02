#pragma once

#include "Assets/AssetStreamer.h"
#include "Renderer/Backends/SdlGpu/Resources/VertexBuffer.h"
#include "Renderer/Common/Resources/Buffers.h"
#include "Renderer/Common/Resources/MeshCache.h"

using namespace Silent::Assets;

namespace Silent::Renderer::SdlGpu
{
    /** @brief GPU mesh cache. */
    class MeshCache : public MeshCacheBase
    {
    private:
        // =======
        // Fields
        // =======

        VertexBuffer<BufferVertex3d> _vertexBuffer = {};

    public:
        // =============
        // Constructors
        // =============

        /** @brief Creates an instance from a reference GPU 3D vertex buffer.
         *
         * @param vertBuffer GPU 3D vertex buffer.
         */
        MeshCache(SDL_GPUDevice& device, int vertCount, int idxCount, const std::string& name);

        // ==========
        // Utilities
        // ==========

        /** @brief Uploads 3D vertices and indices of a mesh to the GPU.
         *
         * @param copyPass GPU copy pass.
         * @param verts 3D vertices.
         * @param idxs 3D vertex indices.
         * @param name Mesh name.
         */
        void Upload(SDL_GPUCopyPass& copyPass,
                    const std::vector<BufferVertex3d>& verts, const std::vector<uint16>& idxs,
                    const std::string& name);

        /** @brief Uploads model meshes from a streamable model asset to the GPU.
         *
         * @param copyPass GPU copy pass.
         * @param assetName Streamable model asset name.
         */
        void Upload(SDL_GPUCopyPass& copyPass, const std::string& assetName);

        /** @brief Binds the cached meshes GPU buffer for drawing.
         *
         * @param renderPass Render pass.
         */
        void Bind(SDL_GPURenderPass& renderPass);

    private:
        // ========
        // Helpers
        // ========

        /** @brief Uploads meshes from an ILM model asset to the GPU.
         *
         * @note By convention, each GPU mesh is named as follows:
         * `[ILM asset name]_[bone name]`.
         *
         * @param copyPass GPU copy pass.
         * @param asset ILM asset.
         */
        void UploadIlm(SDL_GPUCopyPass& copyPass, const Asset& asset);

        /** @brief Uploads meshes from an IPD model asset to the GPU.
         *
         * @param copyPass GPU copy pass.
         * @param asset IPD asset.
         */
        void UploadIpd(SDL_GPUCopyPass& copyPass, const Asset& asset);

        /** @brief Uploads meshes from a PLM model asset to the GPU.
         *
         * @param copyPass GPU copy pass.
         * @param asset PLM asset.
         */
        void UploadPlm(SDL_GPUCopyPass&, const Asset& asset);

        /** @brief Uploads meshes from a TMD model asset to the GPU.
         *
         * @note Each GPU mesh uses the following naming convention:
         * `[TMD asset name]_[mesh index]`.
         *
         * @param copyPass GPU copy pass.
         * @param asset TMD asset.
         */
        void UploadTmd(SDL_GPUCopyPass& copyPass, const Asset& asset);
    };
}
