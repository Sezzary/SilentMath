#include "Framework.h"
#include "Utils/Font.h"

#include "Application.h"
#include "Utils/Utils.h"

namespace Silent::Utils
{
    /** @brief HarfBuzz text shaping data. */
    struct ShapingInfo
    {
        hb_glyph_info_t*     Glyphs    = nullptr;
        hb_glyph_position_t* Positions = nullptr;
        hb_buffer_t*         Buffer    = nullptr;
    };

    Font::Font(FT_Library& fontLib, const FontMetadata& metadata, const std::filesystem::path& path, const std::string& precacheGlyphs)
    {
        constexpr int POINT_SIZE_MAX = ATLAS_SIZE / 8;

        _name               = metadata.Name;
        _enableAntialiasing = metadata.EnableAntialiasing;
        _isAtlasUpdated     = false;

        // Clamp point size.
        _pointSize = metadata.PointSize;
        if (_pointSize > POINT_SIZE_MAX)
        {
            Debug::Log(Fmt("Attempted to initialize font chain `{}` with invalid point size {}. Max is {}.", _name, _pointSize, POINT_SIZE_MAX),
                       Debug::LogLevel::Warning);

            _pointSize = std::min<int>(_pointSize, POINT_SIZE_MAX);
        }

        // Add chained fonts to library.
        _ftFonts.reserve(metadata.Filenames.size());
        for (const auto& filename : metadata.Filenames)
        {
            FT_Face ftFont = nullptr;
            if (FT_New_Face(fontLib, (path / filename).string().c_str(), 0, &ftFont))
            {
                throw std::runtime_error("Failed to initialize font.");
            }
            _ftFonts.push_back(ftFont);

            // Set point size.
            if (FT_Set_Pixel_Sizes(ftFont, 0, _pointSize))
            {
                throw std::runtime_error("Failed to set font point size.");
            }
        }

        // Set scale factor.
        _scaleFactor = (float)_pointSize / (float)_ftFonts.front()->size->metrics.x_ppem;

        // Add first atlas.
        AddAtlas();

        // Precache glyphs.
        auto codePoints = GetCodePoints(precacheGlyphs);
        for (char32 codePoint : codePoints)
        {
            if (Find(_glyphs, codePoint) != nullptr)
            {
                Debug::Log(Fmt("Attempted to precache existing glyph U+{:X} for font chain `{}`. Check precache string for duplicates.", (int)codePoint, _name),
                           Debug::LogLevel::Warning);
                continue;
            }

            CacheGlyph(codePoint);
        }

        // Debug.
        /*for (int i = 0; i < _textureAtlases.size(); i++)
        {
            stbi_write_png((g_App.GetFilesystem().GetAppDirectory() / (_name + Fmt("_Atlas{}.png", i))).string().c_str(), ATLAS_SIZE, ATLAS_SIZE, RGBA_COMP_COUNT, _textureAtlases[i].data(), ATLAS_SIZE * 4);
            break;
        }*/
    }

    Font::~Font()
    {
        for (auto* rectAtlas : _rectAtlases)
        {
            sma_atlas_destroy(rectAtlas);
        }

        for (auto& ftFont : _ftFonts)
        {
            FT_Done_Face(ftFont);
        }
    }

    int Font::GetPointSize() const
    {
        return _pointSize;
    }

    const std::vector<std::vector<byte>>& Font::GetTextureAtlases() const
    {
        return _textureAtlases;
    }

    ShapedText Font::GetShapedText(const std::string& msg)
    {
        // Cache new glyphs.
        bool hasNewGlyphs = false;
        auto codePoints   = GetCodePoints(msg);
        for (char32 codePoint : codePoints)
        {
            if (Find(_glyphs, codePoint) != nullptr)
            {
                continue;
            }

            hasNewGlyphs = true;
            CacheGlyph(codePoint);
        }
        _isAtlasUpdated = hasNewGlyphs;

        //auto shapingInfos = std::vector<ShapingInfo>(_fontCount);
        auto shapedText   = ShapedText{};
        shapedText.Glyphs.reserve(codePoints.size());

        // Build shaped text.
        for (int i = 0; i < codePoints.size(); i++)
        {
            // Run through font chain.
            for (int j = 0; j < 1/*_fontCount*/; j++)
            {
                // Check if glyph is valid.
                int charIdx = FT_Get_Char_Index(_ftFonts[j], codePoints[i]);
                /*if (charIdx == 0)
                {
                    // If no valid glyphs exist, use first font's invalid glyph.
                    if (j < (_fontCount - 1))
                    {
                        continue;
                    }
                    else
                    {
                        j = 0;
                    }
                }*/

                // Compute kerning.
                int kerning = 0;
                if (i < (codePoints.size() - 1))
                {
                    // From kerning table.
                    if (FT_HAS_KERNING(_ftFonts[j]))
                    {
                        int charIdx0 = FT_Get_Char_Index(_ftFonts[j], codePoints[i]);
                        int charIdx1 = FT_Get_Char_Index(_ftFonts[j], codePoints[i + 1]);

                        auto kerningDelta = FT_Vector{};
                        // Pass FT_KERNING_DEFAULT for standard horizontal kerning
                        FT_Get_Kerning(_ftFonts[j], charIdx0, charIdx1, FT_KERNING_DEFAULT, &kerningDelta);
                        kerning = kerningDelta.x;
                    }
                    // From glyph advance.
                    else
                    {
                        kerning = _glyphs[codePoints[i]].Advance;
                    }
                }
                else
                {
                    kerning = 0;
                }

                // Add shaped glyph.
                shapedText.Glyphs.push_back(ShapedGlyph
                {
                    .Metadata = _glyphs[codePoints[i]],
                    .Kerning  = kerning
                });
                shapedText.Width += kerning;
                break;
            }
        }

        return shapedText;
    }

    bool Font::IsAtlasUpdated() const
    {
        return _isAtlasUpdated;
    }

    void Font::SetAtlasUnupdated()
    {
        _isAtlasUpdated = false;
    }

    std::vector<char32> Font::GetCodePoints(const std::string& msg) const
    {
        // Reserve minimum size.
        auto codePoints = std::vector<char32>{};
        codePoints.reserve((msg.size() / 4) + 1);

        // Collect code points.
        utf8::utf8to32(msg.begin(), msg.end(), std::back_inserter(codePoints));
        return codePoints;
    }

    void Font::CacheGlyph(char32 codePoint)
    {
        // Load valid glyph from font chain.
        FT_Face ftFont = nullptr;
        for (int i = 0; i < _ftFonts.size(); i++)
        {
            // Check if glyph is valid.
            uint charIdx = FT_Get_Char_Index(_ftFonts[i], codePoint);
            if (charIdx == 0)
            {
                // If no valid glyphs exist, use first font's invalid glyph.
                if (i < (_ftFonts.size() - 1))
                {
                    continue;
                }
                else
                {
                    i = 0;
                }
            }

            FT_Load_Glyph(_ftFonts[i], charIdx, _enableAntialiasing ? FT_LOAD_DEFAULT : FT_LOAD_NO_HINTING);
            ftFont = _ftFonts[i];
            break;
        }
        Debug::Assert(ftFont != nullptr, Fmt("Failed to cache glyph U+{:X} for font chain `{}`.", (int)codePoint, _name));

        const auto& metrics = ftFont->glyph->metrics;
        auto        size    = Vector2i(FP_FROM(metrics.width, Q6_SHIFT), FP_FROM(metrics.height, Q6_SHIFT)) + Vector2i(GLYPH_PADDING * 2);

        // Add glyph rectangle.
        const auto* rect = sma_item_add(_rectAtlases[_activeAtlasIdx], size.x, size.y);
        if (rect == nullptr)
        {
            Debug::Log(Fmt("Active atlas {} for font chain `{}` is full. Creating new atlas.", _activeAtlasIdx, _name), Debug::LogLevel::Info);

            // Start new atlas.
            AddAtlas();
            _activeAtlasIdx++;
            rect = sma_item_add(_rectAtlases[_activeAtlasIdx], size.x, size.y);
        }
        Debug::Assert(rect != nullptr, Fmt("Failed to add glyph rectangle U+{:X} for font chain `{}`.", (int)codePoint, _name));

        // Register new glyph.
        _glyphs[codePoint] = GlyphMetadata
        {
            .CodePoint = codePoint,
            .AtlasIdx  = _activeAtlasIdx,
            .Position  = Vector2i(sma_item_x(rect), sma_item_y(rect)),// + Vector2i(GLYPH_PADDING),
            .Size      = size,// - Vector2i(GLYPH_PADDING * 2)
            .Offset    = Vector2i(FP_FROM(metrics.horiBearingX, Q6_SHIFT), FP_FROM(metrics.horiBearingY, Q6_SHIFT)),
            .Advance   = (int)metrics.horiAdvance
        };
        const auto& glyph = _glyphs[codePoint];

        // Rasterize.
        FT_Render_Glyph(ftFont->glyph, FT_RENDER_MODE_NORMAL);
        const auto& bitmap     = ftFont->glyph->bitmap;
        byte*       pixelsFrom = (byte*)bitmap.buffer;
        byte*       pixelsTo   = &_textureAtlases.back()[(((glyph.Position.y + GLYPH_PADDING) * ATLAS_SIZE) * RGBA_COMP_COUNT) + ((glyph.Position.x + GLYPH_PADDING) * RGBA_COMP_COUNT)];

        // Copy pixels to atlas.
        for (int y = 0; y < bitmap.rows; y++)
        {
            for (int x = 0; x < bitmap.width; x++)
            {
                byte pixel = pixelsFrom[(bitmap.width * y) + x];
                pixel      = _enableAntialiasing ? pixel : ((uchar)pixel >= FP_COLOR(0.5f)) ? FP_COLOR(1.0f) : FP_COLOR(0.0f);

                int pixelBaseIdx           = ((ATLAS_SIZE * y) * RGBA_COMP_COUNT) + (x * RGBA_COMP_COUNT);
                pixelsTo[pixelBaseIdx + 0] =
                pixelsTo[pixelBaseIdx + 1] =
                pixelsTo[pixelBaseIdx + 2] =
                pixelsTo[pixelBaseIdx + 3] = pixel;
            }
        }
    }

    void Font::AddAtlas()
    {
        _rectAtlases.push_back(sma_atlas_create(ATLAS_SIZE, ATLAS_SIZE));
        _textureAtlases.emplace_back(std::vector<byte>((ATLAS_SIZE * ATLAS_SIZE) * RGBA_COMP_COUNT));
    }

    FontManager::FontManager()
    {
        if (FT_Init_FreeType(&_library))
        {
            Debug::Log("Failed to initialize font library.", Debug::LogLevel::Error);
        }
    }

    FontManager::~FontManager()
    {
        _fonts.clear();
        FT_Done_FreeType(_library);
    }

    Font* FontManager::GetFont(const std::string& name)
    {
        // Check if font exists.
        auto* font = Find(_fonts, name);
        if (font == nullptr)
        {
            Debug::Log(Fmt("Attempted to get missing font chain `{}`.", name), Debug::LogLevel::Warning);
        }

        return font;
    }

    void FontManager::LoadFont(const FontMetadata& metadata, const std::filesystem::path& path, const std::string& glyphPrecache)
    {
        if (metadata.Filenames.empty())
        {
            Debug::Log(Fmt("Attempted to load font chain `{}` without font files.", metadata.Name), Debug::LogLevel::Warning);
            return;
        }

        // Check if font is already loaded.
        if (Find(_fonts, metadata.Name) != nullptr)
        {
            return;
        }

        // Handle load.
        try
        {
            _fonts.emplace(metadata.Name, Font(_library, metadata, path, glyphPrecache));

            Debug::Log(Fmt("Loaded font chain `{}` at point size {}.", metadata.Name, metadata.PointSize));
        }
        catch (const std::runtime_error& ex)
        {
            Debug::Log(Fmt("Failed to load font chain `{}`: {}", metadata.Name, ex.what()), Debug::LogLevel::Error);
        }
    }
}
