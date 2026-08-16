#pragma once

#include "Assets/Loaders/Utils/Lm.h"

namespace Silent::Assets
{
    struct Asset;

    /** @brief PLM asset data. */
    struct PlmAsset
    {
        LmChunk Lm = {};
    };

    /** @brief Parses a PLM asset file.
     *
     * @param filename Absolute asset file path.
     * @return Parsed PLM asset data as a `void` pointer.
     */
    std::shared_ptr<void> ParsePlm(const stdfs::path& filename);

    /** @brief Queues a PLM asset for upload to the GPU as meshes.
     *
     * @param asset PLM asset.
     */
    void QueuePlmGpuUpload(const Asset& asset);

    /** @brief Queues a PLM asset to release from the GPU as meshes.
     *
     * @param asset PLM asset.
     */
    void QueuePlmGpuRelease(const Asset& asset);
}
