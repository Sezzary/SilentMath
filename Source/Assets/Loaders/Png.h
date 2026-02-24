#pragma once

namespace Silent::Assets
{
    struct Asset;

    /** @brief PNG asset data. */
    struct PngAsset
    {
        Vector2i          Resolution  = Vector2i::Zero;
        std::vector<byte> Pixels      = {};
        float             AspectRatio = 0.0f;
    };

    /** @brief Parses a PNG asset file.
     *
     * @param filename Absolute asset file path.
     * @return Parsed PNG asset data as a `void` pointer.
     */
    std::shared_ptr<void> PngParse(const std::filesystem::path& filename);

    /** @brief Queues a PNG asset for upload to the GPU as a texture.
     *
     * @param asset PNG asset.
     */
    void PngQueueGpuUpload(const Asset& asset);

    /** @brief Queues a PNG asset to release from the GPU as a texture.
     *
     * @param asset PNG asset.
     */
    void PngQueueGpuRelease(const Asset& asset);
}
