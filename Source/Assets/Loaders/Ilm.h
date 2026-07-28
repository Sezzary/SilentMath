#pragma once

#include "Assets/Loaders/Utils/Lm.h"

namespace Silent::Assets
{
    struct Asset;

    /** @brief ILM asset data. */
    struct IlmAsset
    {
        LmChunk Lm = {};
    };

    /** @brief Parses an ILM asset file.
     *
     * @param filename Absolute asset file path.
     * @return Parsed ILM asset data as a `void` pointer.
     */
    std::shared_ptr<void> ParseIlm(const stdfs::path& filename);

    /** @brief Queues an ILM asset for upload to the GPU as meshes.
     *
     * @param asset ILM asset.
     */
    void QueueIlmGpuUpload(const Asset& asset);

    /** @brief Queues a ILM asset to release from the GPU as meshes.
     *
     * @param asset ILM asset.
     */
    void QueueIlmGpuRelease(const Asset& asset);
}
