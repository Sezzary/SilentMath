#include "Framework.h"
#include "Assets/Parsers/Plm.h"

#include "Application.h"
#include "Utils/Stream.h"

using namespace Silent::Utils;

namespace Silent::Assets
{
    std::shared_ptr<void> ParsePlm(const std::filesystem::path& filename)
    {
        const auto& fs = g_App.GetFilesystem();

        auto asset = PlmAsset{};

        // Read file.
        auto stream = Stream(filename, true, false);
        if (!stream.IsOpen())
        {
            throw std::runtime_error(Fmt("Failed to open PLM `{}`.",
                                         std::filesystem::relative(filename, fs.GetAssetsDirectory()).string()));
        }

        // @todo

        return std::make_shared<PlmAsset>(std::move(asset));
    }
}
