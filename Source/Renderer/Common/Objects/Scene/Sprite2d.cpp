#include "Framework.h"
#include "Renderer/Common/Objects/Scene/Sprite2d.h"

#include "Application.h"
#include "Assets/Assets.h"
#include "Renderer/Common/Constants.h"
#include "Renderer/Common/Enums.h"
#include "Renderer/Common/Objects/Scene/Shape2d.h"
#include "Renderer/Common/Utils.h"

using namespace Silent::Assets;

namespace Silent::Renderer
{
    Sprite2d Sprite2d::CreateSprite2d(const std::string& texName, const Vector2& uvMin, const Vector2& uvMax,
                                    const Vector2& pos, float rot, const Vector2& scale,
                                    const Color& color0, const Color& color1, const Color& color2, const Color& color3,
                                    int depth, AlignMode alignMode, ScaleMode scaleMode,
                                    BlendMode blendMode)
    {
        return Sprite2d
        {
            .TextureName = texName,
            .UvMin       = uvMin,
            .UvMax       = uvMax,
            .Position    = pos,
            .Rotation    = rot,
            .Scale       = scale,
            .Col0        = color0,
            .Col1        = color1,
            .Col2        = color2,
            .Col3        = color3,
            .Depth       = depth,
            .AlignMd     = alignMode,
            .ScaleMd     = scaleMode,
            .BlendMd     = blendMode
        };
    }

    Sprite2d Sprite2d::CreateSprite2d(const std::string& texName, const Vector2& uvMin, const Vector2& uvMax,
                                      const Vector2& pos, float rot, const Vector2& scale, const Color& color,
                                      int depth, AlignMode alignMode, ScaleMode scaleMode,
                                      BlendMode blendMode)
    {
        return Sprite2d
        {
            .TextureName = texName,
            .UvMin       = uvMin,
            .UvMax       = uvMax,
            .Position    = pos,
            .Rotation    = rot,
            .Scale       = scale,
            .Col0        = color,
            .Col1        = color,
            .Col2        = color,
            .Col3        = color,
            .Depth       = depth,
            .AlignMd     = alignMode,
            .ScaleMd     = scaleMode,
            .BlendMd     = blendMode
        };
    }
}
