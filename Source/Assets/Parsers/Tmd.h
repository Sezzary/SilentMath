#pragma once

#include "Renderer/Common/Enums.h"
#include "Renderer/Common/Resources/Buffers.h"

using namespace Silent::Renderer;

namespace Silent::Assets
{
    /** @brief TMD indexed vertex. */
    struct TmdVertex
    {
        int PositionIdx = 0;
        int NormalIdx   = 0;
        int UvIdx       = 0;
        int ColorIdx    = 0;

        bool operator==(const TmdVertex& vert) const
        {
            return PositionIdx == vert.PositionIdx &&
                   NormalIdx   == vert.NormalIdx   &&
                   UvIdx       == vert.UvIdx       &&
                   ColorIdx    == vert.ColorIdx;
        }
    };

    /** @brief TMD triangle or quad primitive. */
    struct TmdPrimitive
    {
        std::vector<TmdVertex> Vertices = {};
        BlendMode              BlendMd  = BlendMode::Opaque;
        int                    TPage    = 0;
    };

    /** @brief TMD GPU-compatible linear mesh. */
    struct TmdLinearMesh
    {
        std::vector<BufferVertex3d> Vertices    = {};
        std::vector<uint16>         Idxs        = {};
    };

    /** @brief TMD mesh. */
    struct TmdMesh
    {
        std::vector<TmdPrimitive> Primitives  = {};
        std::vector<Vector3>      Positions   = {};
        std::vector<Vector3>      Normals     = {};
        std::vector<Vector2>      Uvs         = {};
        std::vector<Color>        Colors      = {};

        TmdLinearMesh Linear = {};
    };

    /** @brief TMD asset data. */
    struct TmdAsset
    {
        std::vector<TmdMesh> Meshes = {};
    };

    /** @brief Parses a TMD asset file.
     *
     * @param filename Absolute asset file path.
     * @return Parsed TMD asset data as a `void` pointer.
     */
    std::shared_ptr<void> ParseTmd(const std::filesystem::path& filename);
}

namespace std 
{
    template<> struct hash<Silent::Assets::TmdVertex> 
    {
        size_t operator()(const Silent::Assets::TmdVertex& vert) const noexcept 
        {
            size_t hash = 0;
            auto CombineHash = [&](int val)
            {
                hash ^= std::hash<int>{}(val) + Silent::Math::GOLDEN_RATIO + (hash << 6) + (hash >> 2);
            };

            CombineHash(vert.PositionIdx);
            CombineHash(vert.NormalIdx);
            CombineHash(vert.UvIdx);
            CombineHash(vert.ColorIdx);
            return hash;
        }
    };
}
