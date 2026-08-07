#pragma once

namespace Silent::Assets
{
    /** @brief DAT asset data. */
    struct DatAsset
    {
        int Dummy = 0;
    };

    /** @brief Parses an DAT asset file.
     *
     * @param filename Absolute asset file path.
     * @return Parsed DAT asset data as a `void` pointer.
     */
    std::shared_ptr<void> ParseDat(const stdfs::path& filename);
}
