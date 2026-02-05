#pragma once

namespace Silent::Assets
{
    /** @brief ILM asset data. */
    struct IlmAsset
    {
        
    };

    /** @brief Parses an ILM asset file.
     *
     * @param filename Absolute asset file path.
     * @return Parsed ILM asset data as a `void` pointer.
     */
    std::shared_ptr<void> ParseIlm(const std::filesystem::path& filename);
}
