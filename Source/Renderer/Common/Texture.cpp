#include "Framework.h"
#include "Renderer/Common/Texture.h"

#include "Utils/Utils.h"

using namespace Silent::Utils;

namespace Silent::Renderer
{
    ITexture* TextureManagerBase::Get(const std::string& name)
    {
        auto* tex = Find(_textures, name);
        if (tex == nullptr)
        {
            Debug::Log(Fmt("Texture manager attempted to get invalid GPU texture `{}`.", name));
            return nullptr;
        }

        return tex->get();
    }
}
