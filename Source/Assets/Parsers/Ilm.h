#pragma once

namespace Silent::Assets
{
    /** @brief ILM indexed vertex. */
    struct IlmVertex
    {
        int PositionIdx = 0;
        int NormalIdx   = 0;
        int UvIdx       = 0;
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

    /** @brief ILM asset data. */
    struct IlmAsset
    {
        std::vector<IlmMesh> Meshes = {};
        std::vector<int>     Ids    = {};
    };

    /** @brief Parses an ILM asset file.
     *
     * @param filename Absolute asset file path.
     * @return Parsed ILM asset data as a `void` pointer.
     */
    std::shared_ptr<void> ParseIlm(const std::filesystem::path& filename);
}
