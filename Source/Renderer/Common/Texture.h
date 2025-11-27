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

        // ========
        // Getters
        // ========

        /** @brief Gets a cached texture.
         *
         * @param name Texture name.
         * @return Cached texture if it exists, otherwise `nullptr`.
         */
        ITexture* Get(const std::string& name);
    };
}
