#pragma once

namespace Silent::Assets
{
    /** @brief PNG asset data. */
    struct PngAsset
    {
        Vector2i          Resolution = Vector2i::Zero;
        std::vector<byte> Pixels     = {};
    };

    /** @brief Parses a PNG asset file.
     *
     * @param filename Absolute asset file path.
     * @return Parsed PNG asset data as a `void` pointer.
     */
    std::shared_ptr<void> ParsePng(const std::filesystem::path& filename);
}
