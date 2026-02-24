#pragma once

namespace Silent::Assets
{
    struct Asset;

    /** @brief TIM asset data. */
    struct TimAsset
    {
        Vector2i                         Resolution  = Vector2i::Zero;
        std::vector<byte>                Pixels      = {}; // @todo Make this `PixelIdxs`.
        float                            AspectRatio = 0.0f;
        std::vector<std::vector<uint16>> Cluts       = {};
    };

    /** @brief Parses a TIM asset file.
     *
     * @param filename Absolute asset file path.
     * @return Parsed TIM asset data as a `void` pointer.
     */
    std::shared_ptr<void> TimParse(const std::filesystem::path& filename);

    /** @brief Queues a TIM asset for upload to the GPU as a texture.
     *
     * @param asset TIM asset.
     */
    void TimQueueGpuUpload(const Asset& asset);

    /** @brief Queues a TIM asset to release from the GPU as a texture.
     *
     * @param asset TIM asset.
     */
    void TimQueueGpuRelease(const Asset& asset);
}
