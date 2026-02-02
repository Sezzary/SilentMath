#pragma once

namespace Silent::Assets
{
    /** @brief TMD indexed vertex. */
    struct TmdVertex
    {
        int PositionIdx = 0;
        int NormalIdx   = 0;
        int UvIdx       = 0;
        int ColorIdx    = 0;
    };

    /** @brief TMD triangle or quad primitive. */
    struct TmdPrimitive
    {
        std::vector<TmdVertex> Vertices = {};
    };

    /** @brief TMD mesh. */
    struct TmdMesh
    {
        std::string               TextureName = {}; // @todo Unsure how to attach a texture. Maybe write a hardcoded table?
        std::vector<Vector3>      Positions   = {};
        std::vector<Vector3>      Normals     = {};
        std::vector<Vector2>      Uvs         = {};
        std::vector<Color>        Colors      = {};
        std::vector<TmdPrimitive> Primitives  = {};
    };

    /** @brief TMD asset data. */
    struct TmdAsset
    {
        std::vector<TmdMesh> Meshes = {};
    };

    /** @brief Parses a TMD asset file.
     *
     * @param filename Absolute asset file path on the system.
     * @return Parsed TMD asset data as a `void` pointer.
     */
    std::shared_ptr<void> ParseTmd(const std::filesystem::path& filename);
}
