#pragma once

namespace Silent::Assets
{
    /** @brief TMD vertex. */
    struct TmdVertex
    {
        int     PositionIdx = 0;
        int     NormalIdx   = 0;
        Vector2 Uv          = Vector2::Zero;
        Color   Col         = Color::Clear;
    };

    /** @brief TMD triangle primitive. */
    struct TmdTriangle
    {
        std::array<TmdVertex, TRI_VERTEX_COUNT> Vertices = {};
    };

    /** @brief TMD quad primitive. */
    struct TmdQuad
    {
        std::array<TmdVertex, QUAD_VERTEX_COUNT> Vertices = {};
    };

    using TmdPrimitive = std::variant<TmdTriangle,
                                      TmdQuad>;

    /** @brief TMD mesh. */
    struct TmdMesh
    {
        std::vector<Vector3>      Positions  = {};
        std::vector<Vector3>      Normals    = {};
        std::vector<TmdPrimitive> Primitives = {};
    };

    /** @brief TMD asset data. */
    struct TmdAsset
    {
        std::vector<TmdMesh> Meshes      = {};
        std::string          TextureName = {};
    };

    /** @brief Parses a TMD asset file.
     *
     * @param filename Absolute asset file path on the system.
     * @return Parsed TMD asset data as a `void` pointer.
     */
    std::shared_ptr<void> ParseTmd(const std::filesystem::path& filename);
}
