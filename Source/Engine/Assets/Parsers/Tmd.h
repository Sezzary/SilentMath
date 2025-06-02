#pragma once

namespace Silent::Assets
{
    struct TmdAsset
    {

    };

    std::shared_ptr<TmdAsset> ParseTmd(const std::filesystem::path& filename);
}
