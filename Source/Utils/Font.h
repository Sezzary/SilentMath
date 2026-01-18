#pragma once

namespace Silent::Utils
{
    /** @brief Font chain metadata. */
    struct FontMetadata
    {
        std::string              Name               = {};
        std::vector<std::string> Filenames          = {};
        int                      PointSize          = 0;
        float                    KerningScale       = 1.0f;
        bool                     EnableAntialiasing = false;
    };

    /** @brief Rasterized glyph metadata. */
    struct GlyphMetadata
    {
        char32   CodePoint     = 0;
        int      AtlasIdx      = 0;
        Vector2i AtlasPosition = Vector2i::Zero;
        Vector2i AtlasSize     = Vector2i::Zero;

        Vector2 Bearing   = Vector2::Zero;
        float   Advance   = 0.0f;
        float   Ascender  = 0.0f;
        float   Descender = 0.0f;
    };

    /** @brief Shaped glyph data. */
    struct ShapedGlyph
    {
        const GlyphMetadata& Metadata;
        float                Kerning = 0.0f;
    };

    /** @brief Shaped text data. */
    struct ShapedText
    {
        std::vector<ShapedGlyph> Glyphs = {};
        float                    Width  = 0.0f;
    };

    /** @brief Atlased font chain. */
    class Font
    {
    public:
        // ==========
        // Constants
        // ==========

        static constexpr int ATLAS_SIZE = 1024;

    private:
        static constexpr int GLYPH_PADDING = 1;

        // =======
        // Fields
        // =======

        std::string _name               = {};
        int         _pointSize          = 0;
        float       _kerningScale       = 1.0f;
        bool        _enableAntialiasing = false;
        
        std::vector<FT_Face>                      _ftFonts        = {};
        std::unordered_map<char32, GlyphMetadata> _glyphs         = {}; /** Key = code point, value = rasterized glyph metadata. */
        std::vector<smol_atlas_t*>                _rectAtlases    = {};
        std::vector<std::vector<byte>>            _textureAtlases = {};
        int                                       _activeAtlasIdx = 0;

        std::set<int> _dirtyGpuAtlasIdxs = {};

    public:
        // =============
        // Constructors
        // =============

        /** @brief Constructs an uninitialized default `Font`. */
        Font() = default;

        /** @brief Constructs a `Font` from a chain of font files and adds it to a library and precaches a set of glyphs in the texture atlas.
         *
         * @param fontLib Library to load the font into.
         * @param metadata Font chain metadata.
         * @param path Path containing font files.
         * @param precacheGlyphs Glyphs to precache.
         */
        Font(FT_Library& fontLib, const FontMetadata& metadata, const std::filesystem::path& path, const std::string& precacheGlyphs);

        /** @brief Gracefully destroys the `Font` and frees resources. */
        ~Font();

        // ========
        // Getters
        // ========

        /** @brief Gets the point size.
         *
         * @return Point size.
         */
        int GetPointSize() const;

        /** @brief Gets the monochrome texture atlases containing cached font glyphs.
         *
         * @return Glyph texture atlases.
         */
        const std::vector<std::vector<byte>>& GetTextureAtlases() const;

        /** @brief Gets the indices of glyph atlas textures which require updating on the GPU.
         *
         * @return Dirty glyph texture atlas indices.
         */
        const std::set<int>& GetDirtyGpuAtlasIdxs() const;

        /** @brief Gets the shaped text for a message.
         *
         * @param msg Message to parse.
         * @return Shaped text.
         */
        ShapedText GetShapedText(const std::string& msg);

        // ==========
        // Utilities
        // ==========

        /** @brief Clears all indices of glyph texture atlases marked for updating on the GPU. */
        void ClearDirtyGpuAtlasIdxs();

        // ==========
        // Inquirers
        // ==========

    private:
        // ========
        // Helpers
        // ========

        /** @brief Gets the code points for the glyphs in a message.
         *
         * @param msg Message to parse.
         * @return Code points for each glyph.
         */
        std::vector<char32> GetCodePoints(const std::string& msg) const;

        /** @brief Caches a new glyph in the texture atlas.
         *
         * @param codePoint Code point of the glyph to cache.
         */
        void CacheGlyph(char32 codePoint);

        /** @brief Adds a new glyph texture atlas to use for caching. */
        void AddAtlas();
    };

    /** @brief Atlased font manager. */
    class FontManager
    {
    private:
        // =======
        // Fields
        // =======

        FT_Library                            _library = {};
        std::unordered_map<std::string, Font> _fonts   = {}; /** Key = font name, value = atlased font. */

    public:
        // =============
        // Constructors
        // =============

        /** @brief Constructs a default `FontManager`. */
        FontManager();

        /** @brief Gracefully destroys the `FontManager` and frees resources. */
        ~FontManager();

        // ========
        // Getters
        // ========

        /** @brief Gets a loaded font.
         *
         * @param name Name of the font to retrieve.
         * @return Loaded font or `nullptr` if missing.
         */
        Font* GetFont(const std::string& name);

        // ==========
        // Utilities
        // ==========

        /** @brief Loads and registers a font as a font chain.
         *
         * @param metadata Font chain metadata.
         * @param path Path containing font files.
         * @param precacheGlyphs Glyphs to precache in the atlas on font initialization.
         */
        void LoadFont(const FontMetadata& metadata, const std::filesystem::path& path, const std::string& precacheGlyphs = {});
    };
}
