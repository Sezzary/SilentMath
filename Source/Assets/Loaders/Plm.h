#pragma once

namespace Silent::Assets
{
    /** @brief PLM asset data. */
    struct PlmAsset
    {
        
    };

    /** @brief Parses a PLM asset file.
     *
     * @param filename Absolute asset file path.
     * @return Parsed PLM asset data as a `void` pointer.
     */
    std::shared_ptr<void> ParsePlm(const std::filesystem::path& filename);
}
