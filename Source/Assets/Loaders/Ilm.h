#pragma once

#include "Renderer/Common/Resources/Layouts/Buffers.h"

using namespace Silent::Renderer;

namespace Silent::Assets
{
    struct Asset;

    /** @brief ILM indexed vertex. */
    struct IlmVertex
    {
        int PositionIdx = 0;
        int NormalIdx   = 0;
        int UvIdx       = 0;
        int PaletteIdx  = NO_VALUE;

        bool operator==(const IlmVertex& vert) const
        {
            return PositionIdx == vert.PositionIdx &&
                   NormalIdx   == vert.NormalIdx   &&
                   UvIdx       == vert.UvIdx       &&
                   PaletteIdx  == vert.PaletteIdx;
        }
    };

    /** @brief ILM primitive. */
    struct IlmPrimitive
    {
        std::vector<IlmVertex> Vertices = {};
    };

    /** @brief ILM native format mesh. */
    struct IlmNativeMesh
    {
        std::vector<IlmPrimitive> Primitives = {};
        std::vector<Vector3i>     Positions  = {}; /* Q24.7 */
        std::vector<Vector3i>     Normals    = {}; /* Q24.7 */
        std::vector<Vector2i>     Uvs        = {}; /* Q23.8 */
    };

    /** @brief ILM GPU-compatible linear mesh. */
    struct IlmLinearMesh
    {
        std::vector<BufferVertex3d> Vertices = {};
        std::vector<uint16>         Idxs     = {};
    };

    /** @brief ILM mesh. */
    struct IlmMesh
    {
        int           BoneIdx  = 0;
        std::string   BoneName = {};
        IlmNativeMesh Native   = {};
        IlmLinearMesh Linear   = {};
    };

    /** @brief ILM asset data. */
    struct IlmAsset
    {
        std::string          Name        = {};
        std::string          TextureName = {};
        std::vector<IlmMesh> Meshes      = {};
        std::vector<int>     MeshIds     = {};
    };

    /** @brief Parses an ILM asset file.
     *
     * @param filename Absolute asset file path.
     * @return Parsed ILM asset data as a `void` pointer.
     */
    std::shared_ptr<void> ParseIlm(const stdfs::path& filename);

    /** @brief Queues an ILM asset for upload to the GPU as meshes.
     *
     * @param asset ILM asset.
     */
    void IlmQueueGpuUpload(const Asset& asset);

    /** @brief Queues a ILM asset to release from the GPU as meshes.
     *
     * @param asset ILM asset.
     */
    void IlmQueueGpuRelease(const Asset& asset);
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
