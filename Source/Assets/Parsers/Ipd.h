#pragma once

namespace Silent::Assets
{
    /** @brief IPD mesh. */
    struct IpdMesh
    {
        std::string Name = {};

        std::string               TextureName = {}; // @todo Attach texture.
        //std::vector<IlmPrimitive> Primitives  = {};
        std::vector<Vector3>      Positions   = {};
        std::vector<Vector3>      Normals     = {};
        std::vector<Vector2>      Uvs         = {};
    };

    /** @brief IPD asset data. */
    struct IpdAsset
    {
        std::vector<IpdMesh> Meshes = {};
    };

    /** @brief Parses an IPD asset file.
     *
     * @param filename Absolute asset file path.
     * @return Parsed IPD asset data as a `void` pointer.
     */
    std::shared_ptr<void> ParseIpd(const std::filesystem::path& filename);
}
