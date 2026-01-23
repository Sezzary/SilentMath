#pragma once

#include "Renderer/Common/Objects/Scene/Sprite2d.h"
#include "Renderer/Common/Enums.h"

namespace Silent::Utils{ struct ShapedGlyph; }

using namespace Silent::Utils;

namespace Silent::Renderer
{
    constexpr int GLYPH_2D_COUNT_MAX = 1024;

    /** @brief Text embellishment styles. */
    enum class TextStyle
    {
        Flat,
        Gradient
    };

    /** @brief Glyph gradient data. */
    struct GlyphGradient
    {
    };

    /** @brief 2D screen glyph. */
    struct Glyph2d
    {
        std::string AtlasName = {};
        Vector2     UvMin     = Vector2::Zero;
        Vector2     UvMax     = Vector2::Zero;
        Vector2     Position  = Vector2::Zero;
        float       Rotation  = 0.0f;
        Vector2     Scale     = Vector2::Zero;
        Color       Col       = Color::Clear;
        int         Depth     = 0;
        ScaleMode   ScaleMd   = ScaleMode::ShortEdge;

        bool  HasGradient    = false;
        int   GradientSteps  = 0;
        float GradientUvMinY = 0.0f;
        float GradientUvMaxY = 0.0f;

        static Glyph2d CreateGlyph2d(const ShapedGlyph& shapedGlyph, bool hasGradient,
                                     const std::string& atlasName, const Vector2& uvMin, const Vector2& uvMax,
                                     const Vector2& pos, float rot, const Vector2& scale, const Color& color,
                                     int depth = 0, ScaleMode scaleMode = ScaleMode::ShortEdge);
    };

    /** @brief 2D screen text with markup. */
    struct Text2d
    {
        std::string Message      = {};
        std::string FontName     = {};
        Vector2     Position     = Vector2::Zero;
        float       Rotation     = 0.0f;
        float       Scale        = 0.0f;
        float       KerningScale = 0.0f;
        float       Opacity      = 0.0f;
        TextStyle   Style        = TextStyle::Flat;
        bool        HasShadow    = false;
        int         Depth        = 0;
        AlignMode   AlignMd      = AlignMode::Center;
        ScaleMode   ScaleMd      = ScaleMode::ShortEdge;
        BlendMode   BlendMd      = BlendMode::Alpha;

        static Text2d CreateText2d(const std::string& msg, const std::string& fontName,
                                   const Vector2& pos, float rot, float scale, float kerningScale, float opacity,
                                   TextStyle style, bool hasDropShadow,
                                   int depth = 0, AlignMode alignMode = AlignMode::Center, ScaleMode scaleMode = ScaleMode::ShortEdge,
                                   BlendMode blendMode = BlendMode::Alpha);
    };
}
