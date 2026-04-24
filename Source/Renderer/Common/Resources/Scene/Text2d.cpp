#include "Framework.h"
#include "Renderer/Common/Resources/Scene/Text2d.h"

#include "Application.h"
#include "Renderer/Common/Enums.h"
#include "Utils/Font.h"

using namespace Silent::Utils;

namespace Silent::Renderer
{
    Glyph2d Glyph2d::CreateGlyph2d(const ShapedGlyph& shapedGlyph, bool hasGradient,
                                   const std::string& atlasName, const Vector2& uvMin, const Vector2& uvMax,
                                   const Vector2& pos, float rot, const Vector2& scale, const Color& color,
                                   int depth, ScaleMode scaleMode)
    {
        return Glyph2d
        {
            .AtlasName      = atlasName,
            .UvMin          = uvMin,
            .UvMax          = uvMax,
            .Position       = pos,
            .Rotation       = rot,
            .Scale          = scale,
            .Col            = color,
            .Depth          = depth,
            .ScaleMd        = scaleMode,
            .HasGradient    = hasGradient,
            .GradientSteps  = 0,
            .GradientUvMinY = Remap(shapedGlyph.Attribs.Ascender,
                                    shapedGlyph.Attribs.MinY, shapedGlyph.Attribs.MaxY,
                                    uvMax.y, uvMin.y), // @todo Ascender value has issues with high-res fonts?
            .GradientUvMaxY = Remap(0.0f,
                                    shapedGlyph.Attribs.MinY, shapedGlyph.Attribs.MaxY,
                                    uvMax.y, uvMin.y)
        };
    }

    Text2d Text2d::CreateText2d(const std::string& msg, const std::string& fontName,
                                const Vector2& pos, float rot, float scale, float tracking,
                                const Color& color, TextStyle style, bool hasDropShadow,
                                int depth, AlignMode alignMode, ScaleMode scaleMode,
                                BlendMode blendMode)
    {
        auto& fonts = g_App.GetFonts();

        // Get font.
        auto* font = fonts.GetFont(fontName);
        if (font == nullptr)
        {
            Debug::Log(Fmt("Attempted to create 2D text with missing font `{}`.", fontName),
                       Debug::LogLevel::Warning, Debug::LogMode::Debug);
            return {};
        }

        // Get shaped text glyphs.
        return Text2d
        {
            .Shape     = font->GetShapedText(msg),
            .Message   = msg,
            .Font      = font,
            .Position  = pos,
            .Rotation  = rot,
            .Scale     = scale,
            .Tracking  = tracking,
            .Col       = color,
            .Style     = style,
            .HasShadow = hasDropShadow,
            .Depth     = depth,
            .AlignMd   = alignMode,
            .ScaleMd   = ScaleMode::HorizontalEdge, // @todo
            .BlendMd   = blendMode
        };
    }
}
