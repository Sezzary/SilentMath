#include "Framework.h"
#include "Engine/Assets/Parsers/Tmd.h"

namespace Silent::Assets
{
    std::shared_ptr<TmdAsset> ParseTmd(const std::filesystem::path& filename)
    {
        return std::make_shared<TmdAsset>();
    }
}
