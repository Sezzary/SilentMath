#pragma once

#include "Input/Input.h"

using namespace Silent::Input;

namespace Silent::Assets
{
    /** @brief DAT asset data. */
    struct DatAsset
    {
        Recording InputRecording = {};
        uint      RandomSeed     = 0;
    };

    /** @brief Parses an DAT asset file.
     *
     * @param filename Absolute asset file path.
     * @return Parsed DAT asset data as a `void` pointer.
     */
    std::shared_ptr<void> ParseDat(const stdfs::path& filename);
}
