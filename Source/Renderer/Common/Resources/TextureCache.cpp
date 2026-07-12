#include "Framework.h"
#include "Renderer/Common/Resources/TextureCache.h"

#include "Utils/Utils.h"

using namespace Silent::Utils;

namespace Silent::Renderer
{
    std::vector<std::string> TextureCacheBase::GetNames() const
    {
        // Collect sorted texture names.
        auto names = std::vector<std::string>{};
        names.reserve(_textures.size());
        for (const auto& [name, tex] : _textures)
        {
            names.push_back(name);
        }
        Sort(names);

        return names;
    }
}
