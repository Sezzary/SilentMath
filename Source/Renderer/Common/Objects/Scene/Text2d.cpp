#include "Framework.h"
#include "Renderer/Common/Objects/Scene/Text2d.h"

#include "Renderer/Common/Enums.h"

namespace Silent::Renderer
{
    Text2d Text2d::CreateText2d(const std::string& msg, const std::string& fontName,
                                const Vector2& pos, float rot, float scale, float kernScale, float opacity,
                                TextStyle style, bool hasDropShadow,
                                int depth, ScaleMode scaleMode, BlendMode blendMode)
    {
        return Text2d
        {
            .Message      = msg,
            .FontName     = fontName,
            .Position     = pos,
            .Rotation     = rot,
            .Scale        = scale,
            .KerningScale = kernScale,
            .Opacity      = opacity,
            .Style        = style,
            .HasShadow    = hasDropShadow,
            .Depth        = depth,
            .ScaleMd      = scaleMode,
            .BlendMd      = blendMode
        };
    }
}
