#include "Framework.h"
#include "Engine/Services/Assets/Parsers/Plm.h"

namespace Silent::Assets
{
    std::shared_ptr<void> ParsePlm(const std::filesystem::path& filename)
    {
        // TODO
        return std::make_shared<PlmAsset>();
    }
}
