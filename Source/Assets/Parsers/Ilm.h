#pragma once

#include "Renderer/Common/Resources/Buffers.h"

using namespace Silent::Renderer;

namespace Silent::Assets
{
    /** @brief ILM indexed vertex. */
    struct IlmVertex
    {
        int PositionIdx = 0;
        int NormalIdx   = 0;
        int UvIdx       = 0;

        bool operator==(const IlmVertex& vert) const
        {
            return PositionIdx == vert.PositionIdx &&
                   NormalIdx   == vert.NormalIdx   &&
                   UvIdx       == vert.UvIdx;
        }
    };

    /** @brief ILM primitive. */
    struct IlmPrimitive
    {
        std::vector<IlmVertex> Vertices = {};
    };

    /** @brief ILM mesh. */
    struct IlmMesh
    {
        std::vector<IlmPrimitive> Primitives  = {};
        std::vector<Vector3>      Positions   = {};
        std::vector<Vector3>      Normals     = {};
        std::vector<Vector2>      Uvs         = {};
        std::string               TextureName = {}; // @todo Unsure how to attach a texture. Maybe write a hardcoded table?
    };

    /** @brief ILM GPU-compatible linear mesh. */
    struct IlmLinearMesh
    {
        std::vector<BufferVertex3d> Vertices    = {};
        std::vector<uint16>         Idxs        = {};
        std::string                 TextureName = {};
    };

    /** @brief ILM asset data. */
    struct IlmAsset
    {
        std::vector<IlmMesh>       Meshes       = {};
        std::vector<IlmLinearMesh> LinearMeshes = {};
        std::vector<int>           Ids          = {};
    };

    /** @brief Parses an ILM asset file.
     *
     * @param filename Absolute asset file path.
     * @return Parsed ILM asset data as a `void` pointer.
     */
    std::shared_ptr<void> ParseIlm(const std::filesystem::path& filename);
}

namespace std 
{
    template<> struct hash<Silent::Assets::IlmVertex> 
    {
        size_t operator()(const Silent::Assets::IlmVertex& vert) const noexcept 
        {
            size_t hash = 0;
            auto CombineHash = [&](int val)
            {
                hash ^= std::hash<int>{}(val) + Silent::Math::GOLDEN_RATIO + (hash << 6) + (hash >> 2);
            };

            CombineHash(vert.PositionIdx);
            CombineHash(vert.NormalIdx);
            CombineHash(vert.UvIdx);
            return hash;
        }
    };
}
