#pragma once

#include "Renderer/Common/Resources/Layouts/Buffers.h"

using namespace Silent::Renderer;

namespace Silent::Assets
{
    struct Asset;

    /** @brief LM indexed vertex. */
    struct LmVertex
    {
        int PositionIdx = 0;
        int NormalIdx   = 0;
        int UvIdx       = 0;
        int PaletteIdx  = NO_VALUE;

        bool operator==(const LmVertex& vert) const
        {
            return PositionIdx == vert.PositionIdx &&
                   NormalIdx   == vert.NormalIdx   &&
                   UvIdx       == vert.UvIdx       &&
                   PaletteIdx  == vert.PaletteIdx;
        }
    };

    /** @brief LM primitive. */
    struct LmPrimitive
    {
        std::vector<LmVertex> Vertices = {};
    };

    /** @brief LM native format mesh. */
    struct LmNativeMesh
    {
        std::vector<LmPrimitive> Primitives = {};
        std::vector<Vector3i>     Positions  = {}; /* Q24.7 */
        std::vector<Vector3i>     Normals    = {}; /* Q24.7 */
        std::vector<Vector2i>     Uvs        = {}; /* Q23.8 */
    };

    /** @brief LM GPU-compatible linear mesh. */
    struct LmLinearMesh
    {
        std::vector<BufferVertex3d> Vertices = {};
        std::vector<uint16>         Idxs     = {};
    };

    /** @brief LM mesh. */
    struct LmMesh
    {
        int           BoneIdx  = 0;
        std::string   BoneName = {};
        LmNativeMesh Native   = {};
        LmLinearMesh Linear   = {};
    };

    /** @brief LM asset data. */
    struct LmAsset
    {
        std::string          Name        = {};
        std::string          TextureName = {};
        std::vector<LmMesh> Meshes      = {};
        std::vector<int>     MeshIds     = {};
    };

    /** @brief Parses an LM asset file.
     *
     * @param filename Absolute asset file path.
     * @return Parsed LM asset data as a `void` pointer.
     */
    std::shared_ptr<void> ParseLm(const stdfs::path& filename);

    /** @brief Queues an LM asset for upload to the GPU as meshes.
     *
     * @param asset LM asset.
     */
    void QueueLmGpuUpload(const Asset& asset);

    /** @brief Queues a LM asset to release from the GPU as meshes.
     *
     * @param asset LM asset.
     */
    void QueueLmGpuRelease(const Asset& asset);
}

namespace std 
{
    template<> struct hash<Silent::Assets::LmVertex> 
    {
        size_t operator()(const Silent::Assets::LmVertex& vert) const noexcept 
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
