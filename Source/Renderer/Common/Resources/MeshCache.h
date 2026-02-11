#pragma once

#include "Utils/BlockAllocator.h"

using namespace Silent::Utils;

namespace Silent::Renderer
{
    /** @brief GPU mesh. */
    struct Mesh
    {
        uint32 VertexOffset = 0;
        uint32 IdxOffset    = 0;
        uint32 IdxCount     = 0;
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

        /** @brief Clears all cached meshes. */
        void Clear();

        // ==========
        // Operators
        // ==========

        const Mesh* operator[](const std::string& name) const;
    };
}
