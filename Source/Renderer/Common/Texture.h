#pragma once

namespace Silent::Renderer
{
    /** @brief GPU texture interface. */
    class ITexture abstract
    {
    public:
        // =============
        // Constructors
        // =============

        virtual ~ITexture() = default;
    };

    /** @brief GPU texture cache manager base. */
    class TextureManagerBase abstract
    {
    protected:
        // =======
        // Fields
        // =======

        std::unordered_map<std::string, std::unique_ptr<ITexture>> _textures = {}; /** Key = texture name, value = GPU texture. */

    public:
        // =============
        // Constructors
        // =============

        virtual ~TextureManagerBase() = default;
    };
}
