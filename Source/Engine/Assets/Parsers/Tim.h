#pragma once

namespace Silent::Assets
{
    struct TimAsset
    {
        uint               ColorDepth = 0;
        Vector2i           Resolution = Vector2i::Zero;
        std::vector<uchar> Pixels     = {};
    };

    std::shared_ptr<void> ParseTim(const std::filesystem::path& filename);
}
