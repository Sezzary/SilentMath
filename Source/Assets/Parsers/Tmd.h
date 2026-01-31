#pragma once

namespace Silent::Assets
{
    /** @brief TMD triangle primitive. */
    struct TmdTriangle
    {
        std::array<int, 3> VertexIdxs = {};
        std::array<int, 3> NormalIdxs = {};

        std::string TextureName = {};
    };

    /** @brief TMD quad primitive. */
    struct TmdQuad
    {
        std::array<int, 4> VertexIdxs = {};
        std::array<int, 4> NormalIdxs = {};

        std::string TextureName = {};
    };

    using TmdPrimitive = std::variant<TmdTriangle,
                                      TmdQuad>;

    struct TmdMesh
    {
        std::vector<Vector3>      Vertices   = {};
        std::vector<Vector3>      Normals    = {};
        std::vector<TmdPrimitive> Primitives = {};
    };

    /** @brief TMD asset data. */
    struct TmdAsset
    {
        std::vector<TmdMesh> Meshes = {};
    };

    /** @brief Parses a TMD file to a usable asset.
     *
     * @param filename Absolute asset file path on the system.
     * @return Parsed TMD asset data as a `void` pointer.
     */
    std::shared_ptr<void> ParseTmd(const std::filesystem::path& filename);
}
