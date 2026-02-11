#include "Framework.h"
#include "Renderer/Common/Resources/MeshCache.h"

#include "Application.h"
#include "Assets/AssetStreamer.h"
#include "Utils/BlockAllocator.h"
#include "Utils/Utils.h"

using namespace Silent::Assets;
using namespace Silent::Utils;

namespace Silent::Renderer
{
    void MeshCacheBase::Unload(const std::string& name)
    {
        const auto* mesh = Find(_meshes, name);
        if (mesh == nullptr)
        {
            return;
        }

        _vertexAllocator.Deallocate(mesh->VertexOffset);
        _idxAllocator.Deallocate(mesh->IdxOffset);
        _meshes.erase(name);
    }

    void MeshCacheBase::Clear()
    {
        _meshes.clear();
        _vertexAllocator.Clear();
        _idxAllocator.Clear();
    }

    const Mesh* MeshCacheBase::operator[](const std::string& name) const
    {
        auto* mesh = Find(_meshes, name);
        if (mesh == nullptr)
        {
            Debug::Log(Fmt("Attempted to get missing GPU mesh `{}`.", name), Debug::LogLevel::Warning);
            return nullptr;
        }

        return &*mesh;
    }
}
