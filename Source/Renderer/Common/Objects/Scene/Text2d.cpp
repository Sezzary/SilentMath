#include "Framework.h"
#include "Renderer/Common/Objects/Scene/Text2d.h"

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
                                    uvMax.y, uvMin.y),
            .GradientUvMaxY = Remap(0.0f,
                                    shapedGlyph.Attribs.MinY, shapedGlyph.Attribs.MaxY,
                                    uvMax.y, uvMin.y)
        };
    }

    Text2d Text2d::CreateText2d(const std::string& msg, const std::string& fontName,
                                const Vector2& pos, float rot, float scale, float kerningScale, float opacity,
                                TextStyle style, bool hasDropShadow,
                                int depth, AlignMode alignMode, ScaleMode scaleMode,
                                BlendMode blendMode)
    {
        return Text2d
        {
            .Message      = msg,
            .FontName     = fontName,
            .Position     = pos,
            .Rotation     = rot,
            .Scale        = scale,
            .KerningScale = kerningScale,
            .Opacity      = opacity,
            .Style        = style,
            .HasShadow    = hasDropShadow,
            .Depth        = depth,
            .AlignMd      = alignMode,
            .ScaleMd      = scaleMode,
            .BlendMd      = blendMode
        };
    }
}
