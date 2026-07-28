#pragma once

#include "Assets/Loaders/Utils/Lm.h"

namespace Silent::Assets
{
    struct Asset;

    /** @brief IPD asset data. */
    struct IpdAsset
    {
        LmChunk Lm = {};
    };

    /** @brief Parses an IPD asset file.
     *
     * @param filename Absolute asset file path.
     * @return Parsed IPD asset data as a `void` pointer.
     */
    std::shared_ptr<void> ParseIpd(const stdfs::path& filename);

    /** @brief Queues an IPD asset for upload to the GPU as meshes.
     *
     * @param asset IPD asset.
     */
    void QueueIpdGpuUpload(const Asset& asset);

    /** @brief Queues a IPD asset to release from the GPU as meshes.
     *
     * @param asset IPD asset.
     */
    void QueueIpdGpuRelease(const Asset& asset);
}
