#pragma once

namespace Silent::Renderer
{
    /** @brief GPU texture base. */
    class TextureBase
    {
    protected:
        // =======
        // Fields
        // =======

        Vector2i _resolution = Vector2i::Zero;

    public:
        // =============
        // Constructors
        // =============

        virtual ~TextureBase() = default;
    };

    /** @brief GPU texture cache base. */
    class TextureCacheBase
    {
    protected:
        // =======
        // Fields
        // =======

        std::unordered_map<std::string, std::unique_ptr<TextureBase>> _textures = {}; /** Key = texture name, value = GPU texture. */

    public:
        // =============
        // Constructors
        // =============

        virtual ~TextureCacheBase() = default;
    };
}
