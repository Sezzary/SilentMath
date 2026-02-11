#pragma once

#include "Renderer/Common/Resources/MeshCache.h"

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

        MeshCache(VertexBuffer<BufferVertex3d>& vertBuffer);

        void Load(const std::string& assetName);
    };
}
