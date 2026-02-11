#pragma once

#include "Renderer/Common/Resources/MeshCache.h"

namespace Silent::Renderer
{
    /** @brief GPU mesh cache. */
    class MeshCache : public MeshCacheBase
    {
    private:
        // =======
        // Fields
        // =======

        //VertexBuffer<BufferVertex3d>* _vertexBuffer = nullptr;

    public:
        // =============
        // Constructors
        // =============

        MeshCache(int vertCount, int idxCount);

        void Load(const std::string& assetName);
    };
}
